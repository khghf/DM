#include "DMPCH.h"
#ifdef DM_PLATFORM_WINDOWS
#include "Platform/Window/AppWindow.h"
#include"Core/Log.h"
#include<Core/EventBus/Event/WindowEvent.h>
#include<Core/EventBus/Event/MouseEvent.h>
#include<Core/EventBus/Event/KeyEvent.h>
//#include<Core/EventBus/EventMgr.h>
#include<Core/EventBus/EventBus.h>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
namespace DM
{
	static GLFWwindow* glWindow = nullptr;

	AppWindow::AppWindow(const WindowProps& Props)
	{
		m_WindowProps = Props;
		Init(Props);
	}

	AppWindow::~AppWindow()
	{
		glfwDestroyWindow(glWindow);
		glfwTerminate();
	}

	void AppWindow::Update(float DeltaTime)
	{
		glfwSwapBuffers(glWindow);
	}

	void AppWindow::OnRefresh()
	{
		if (m_RefreshCallback)
			m_RefreshCallback();
	}

	void AppWindow::SetVSync(bool bEnanle)
	{
		if (bEnanle)glfwSwapInterval(1);
		else glfwSwapInterval(0);
		bIsEnableVSync = bEnanle;
	}

	void AppWindow::Init(const WindowProps& Props)
	{
		if (!glfwInit())
		{
			std::cerr << "[GLFW] failed to initialize\n";
			abort();
		}
#ifdef  ENABLE_VULKAN_API
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif

#ifdef ENABLE_OPENGL_API
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif 

		glWindow = glfwCreateWindow(Props.Width, Props.Height, Props.Title.c_str(), nullptr, nullptr);
		if (glWindow == nullptr)
		{
			DM_CORE_ASSERT(false, "{}", "Failed to Create GLFW Window");
			glfwTerminate();
			return;  
		}
		glfwSetWindowTitle(glWindow,Props.Title.c_str());
		glfwSetWindowSize(glWindow, Props.Width, Props.Height);

		// 恢复上次的窗口位置与最大化状态(来自 EngineSettings，PosX/PosY 为 -1 时不定位)
		if (Props.PosX >= 0 && Props.PosY >= 0)
			glfwSetWindowPos(glWindow, Props.PosX, Props.PosY);
		if (Props.Maximized)
			glfwMaximizeWindow(glWindow);

		glfwMakeContextCurrent(glWindow);
		glfwSetWindowUserPointer(glWindow, this);

#ifdef ENABLE_OPENGL_API
		int GladStatus = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		DM_CORE_ASSERT(GladStatus, "{}", "Failed to initialze Glad");
#endif // ENABLE_OPENGL_API


		SetVSync(Props.VSync);
		glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


		{
			//窗口大小回调
			glfwSetWindowSizeCallback
			(glWindow, 
				[](GLFWwindow* window, int width, int height) 
				{ 
					WindowResize e{};
					e.Data.size.x = (float)width;
					e.Data.size.y = (float)height;
					e.bHandled = false;
					EventBus::SendEvent(e);
				}
			);

			// GLFW 将其转为 refresh callback，我们在此强制渲染以防止拖动时内容不刷新
			glfwSetWindowRefreshCallback
			(glWindow,
				[](GLFWwindow* window)
				{
					// 通过 user pointer 获取 Window 实例并调用 OnRefresh
					AppWindow* self = static_cast<AppWindow*>(glfwGetWindowUserPointer(window));
					if (self)self->OnRefresh();
				}
			);


			glfwSetWindowCloseCallback
			(glWindow,
				[](GLFWwindow* window)
				{
					WindowClose e{};
					e.Data.window = window;
					EventBus::SendEvent(e);
				}
			);

			//鼠标按键回调
			glfwSetMouseButtonCallback
			(glWindow,
				[](GLFWwindow* window, int button, int action, int mods)
				{
					MouseClick eClick{};
					MousePress ePress{};
					MouseRelease eRelease{};

					static int LastAction = -1;
					static double x = 0;
					static double y = 0;

					glfwGetCursorPos(window,&x,&y);
					auto set = [&](MouseClick*e) 
						{
							e->Data.button = button;
							e->Data.action = action;
							e->Data.mods = mods;
							e->Data.pos.x = (float)x;
							e->Data.pos.y = (float)y;
						};
					if (LastAction == GLFW_PRESS && action == GLFW_RELEASE)
					{
						set(&eRelease);
						set(&eClick);
						EventBus::SendEvent(eRelease);
						EventBus::SendEvent(eClick);

					}
					else
					{
						switch (action)
						{
						case GLFW_PRESS:
						{
							set(&ePress);
							EventBus::SendEvent(ePress);

							break;
						}
						case GLFW_RELEASE:
						{
							set(&eRelease);
							EventBus::SendEvent(eRelease);
							break;
						}
						default:
							break;
						}
					}
					LastAction = action;
				}
			);
			//鼠标滚轮回调
			glfwSetScrollCallback
			(glWindow,
				[](GLFWwindow* window, double xoffset, double yoffset)
				{
					static MouseScroll e;
					e.Data.offset.x = (float)xoffset;
					e.Data.offset.y = (float)yoffset;
					EventBus::SendEvent(e);

				}
			);

			//键盘按键回调
			glfwSetKeyCallback
			(glWindow,
				[](GLFWwindow* window, int key, int scancode, int action, int mods)
				{
					KeyClick eClick;
					KeyPress ePress;
					KeyRepeat eRepeat;
					KeyRelease eRelease;
					static int LastAction = -1;

					auto set = [&](KeyClick* e) 
						{
						e->Data.key = key;
						e->Data.action = action;
						e->Data.mods = mods;
						e->Data.scancode = scancode; 
						};
					if (LastAction == GLFW_PRESS&&action==GLFW_RELEASE)
					{
						set(&eRelease);
						set(&eClick);
						EventBus::SendEvent(eRelease);
						EventBus::SendEvent(eClick);
					}
					else
					{
						switch (action)
						{
						case GLFW_PRESS:
						{
							set(&ePress);
							EventBus::SendEvent(ePress);
							break;
						}
						case GLFW_REPEAT:
						{
							set(&eRepeat);
							EventBus::SendEvent(eRepeat);
							break;
						}
						case GLFW_RELEASE:
						{
							set(&eRelease);
							EventBus::SendEvent(eRelease);
							break;
						}
						default:
							break;
						}
					}
					LastAction = action;
				}
			);

			//
			glfwSetCharCallback
			(glWindow,
				[](GLFWwindow* window, unsigned int codepoint)
				{
					static KeyTyped e;
					e.Data.KeyCode = codepoint;
					EventBus::SendEvent(e);
				}
			);

			//光标移动回调
			glfwSetCursorPosCallback
			(glWindow,
				[](GLFWwindow* window, double xpos, double ypos)
				{
					static MouseMove e;
					e.Data.pos.x = (float)xpos;
					e.Data.pos.y = (float)ypos;
					EventBus::SendEvent(e);
				}
			);
		}
	}
	UPtr<AppWindow> AppWindow::Create(const WindowProps& Props)
	{
		return UPtr<AppWindow>(new AppWindow(Props));
	}

	bool AppWindow::ShouldClose()
	{
		return glfwWindowShouldClose(glWindow);
	}
	void* AppWindow::GetNativeWindow() const
	{
		return glWindow;
	}

	bool AppWindow::IsWindowFocused() const
	{
		return glfwGetWindowAttrib(glWindow, GLFW_FOCUSED) == GLFW_TRUE;
	}

	void AppWindow::GetWindowSize(int& width, int& height) const
	{
		glfwGetWindowSize(glWindow, &width, &height);
	}

	void AppWindow::GetWindowPos(int& x, int& y) const
	{
		glfwGetWindowPos(glWindow, &x, &y);
	}

	bool AppWindow::IsMaximized() const
	{
		return glfwGetWindowAttrib(glWindow, GLFW_MAXIMIZED) == GLFW_TRUE;
	}
}
#endif // DM_PLATFORM_WINDOW



