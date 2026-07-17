#include "DMPCH.h"
#ifdef DM_PLATFORM_WINDOWS
#include "Platform/Window/Window.h"
#include"Core/Log.h"
#include<Core/EventSystem/Event/WindowEvent.h>
#include<Core/EventSystem/Event/MouseEvent.h>
#include<Core/EventSystem/Event/KeyEvent.h>
#include<Core/EventSystem/EventMgr.h>
#include<Platform/Render/OpenGl/OpenGlContext.h>
#include<Core/Render/Renderer/RenderCommand.h>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
namespace DM
{
	static GLFWwindow* glWindow = nullptr;

	Window::Window(const WindowProps& Props)
	{
		m_WindowProps = Props;
		Init(Props);
	}

	Window::~Window()
	{
		glfwDestroyWindow(glWindow);
		glfwTerminate();
	}

	void Window::Update(float DeltaTime)
	{
		glfwSwapBuffers(glWindow);
	}

	void Window::OnRefresh()
	{
		if (m_RefreshCallback)
			m_RefreshCallback();
	}

	void Window::SetVSync(bool bEnanle)
	{
		if (bEnanle)glfwSwapInterval(1);
		else glfwSwapInterval(0);
		bIsEnableVSync = bEnanle;
	}

	void Window::Init(const WindowProps& Props)
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		//glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // 调试模式，方便看扩展日志
		glWindow = glfwCreateWindow(Props.Width, Props.Height, Props.Title.c_str(), nullptr, nullptr);
		if (glWindow == nullptr)
		{
			DM_CORE_ASSERT(false, "{}", "Failed to Create GLFW Window");
			glfwTerminate();
			return;  // 窗口创建失败，提前返回，避免后续空指针崩溃
		}

		glfwMakeContextCurrent(glWindow);
		glfwSetWindowUserPointer(glWindow, this);
		int GladStatus = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		DM_CORE_ASSERT(GladStatus, "{}", "Failed to initialze Glad");


		SetVSync(true);
		glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		{
			//窗口大小回调
			glfwSetWindowSizeCallback
			(glWindow, 
				[](GLFWwindow* window, int width, int height) 
				{ 
					static WindowResize e;
					e.Data.size.x = (float)width;
					e.Data.size.y = (float)height;
					EventMgr::GetInst()->OnEvent(&e);
				}
			);
			// 窗口刷新回调 —— 在窗口 resize/移动期间，Windows 会发送 WM_PAINT，
			// GLFW 将其转为 refresh callback，我们在此强制渲染以防止拖动时内容不刷新
			glfwSetWindowRefreshCallback
			(glWindow,
				[](GLFWwindow* window)
				{
					// 通过 user pointer 获取 Window 实例并调用 OnRefresh
					Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
					if (self)
						self->OnRefresh();
				}
			);
			glfwSetWindowCloseCallback
			(glWindow,
				[](GLFWwindow* window)
				{
					WindowClose e;
					e.Data.window = window;
					EventMgr::GetInst()->OnEvent(&e);
				}
			);
			//鼠标按键回调
			glfwSetMouseButtonCallback
			(glWindow,
				[](GLFWwindow* window, int button, int action, int mods)
				{
					static MouseClick ec;
					static MousePress ep;
					static MouseRelease er;
					static int LastAction = -1;
					static double x = 0;
					static double y = 0;
					glfwGetCursorPos(window,&x,&y);
					auto set = [&](MouseClick*e) {
							e->Data.button = button;
							e->Data.action = action;
							e->Data.mods = mods;
							e->Data.pos.x = (float)x;
							e->Data.pos.y = (float)y;
						};
					if (LastAction == GLFW_PRESS && action == GLFW_RELEASE)
					{
						set(&er);
						set(&ec);
						EventMgr::GetInst()->OnEvent(&er);
						EventMgr::GetInst()->OnEvent(&ec);
					}
					else
					{
						switch (action)
						{
						case GLFW_PRESS:
						{
							set(&ep);
							EventMgr::GetInst()->OnEvent(&ep);
							break;
						}
						case GLFW_RELEASE:
						{
							set(&er);
							EventMgr::GetInst()->OnEvent(&er);
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
					EventMgr::GetInst()->OnEvent(&e);
				}
			);
			//键盘按键回调
			glfwSetKeyCallback
			(glWindow,
				[](GLFWwindow* window, int key, int scancode, int action, int mods)
				{
					static KeyClick ec;
					static KeyPress ep;
					static KeyRepeat ert;
					static KeyRelease ere;
					static int LastAction = -1;
					auto set = [&](KeyClick* e) {
						e->Data.key = key;
						e->Data.action = action;
						e->Data.mods = mods;
						e->Data.scancode = scancode; 
						};
					if (LastAction == GLFW_PRESS&&action==GLFW_RELEASE)
					{
						set(&ere);
						set(&ec);
						EventMgr::GetInst()->OnEvent(&ere);
						EventMgr::GetInst()->OnEvent(&ec);
					}
					else
					{
						switch (action)
						{
						case GLFW_PRESS:
						{
							set(&ep);
							EventMgr::GetInst()->OnEvent(&ep);
							break;
						}
						case GLFW_REPEAT:
						{
							set(&ert);
							EventMgr::GetInst()->OnEvent(&ert);
							break;
						}
						case GLFW_RELEASE:
						{
							set(&ere);
							EventMgr::GetInst()->OnEvent(&ere);
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
					EventMgr::GetInst()->OnEvent(&e);
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
					EventMgr::GetInst()->OnEvent(&e);
				}
			);
		}
	}
	UPtr<Window> Window::Create(const WindowProps& Props)
	{
		return UPtr<Window>(new Window(Props));
	}

	bool Window::ShouldClose()
	{
		return glfwWindowShouldClose(glWindow);
	}
	void* Window::GetNativeWindow() const
	{
		return glWindow;
	}
}
#endif // DM_PLATFORM_WINDOW



