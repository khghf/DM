#include "DMPCH.h"
#include "Engine.h"
#include <GLFW/glfw3.h>
#include <Core/EventBus/Event/WindowEvent.h>
#include <Platform/Input/Input.h>
#include <Core/Config/ConfigMgr.h>
#include <Core/Config/EngineSettings.h>
#include <Platform/Window/AppWindow.h>
#include "Core/Log.h"
#include <EngineConfig.h>
#include<Core/RHI/RHIDevice.h>
#include<Core/Renderer/TriangleRenderer.h>
namespace DM 
{

	Engine* Engine::m_Inst = nullptr;

	Engine::Engine() : m_Running(false), m_Minimized(false) {}

	Engine::~Engine()
	{
		Shutdown();

		RHI::RHIDevice::ShutDown();
		m_Inst = nullptr;
	}

	Engine*Engine::Create()
	{
		if (!m_Inst)
		{
			m_Inst = new Engine();
		}
		return Get();
	}

	void Engine::Init(const EngineConfig& config)
	{
		Log::Init();

		Paths::Init();          // 幂等：APP 已初始化则跳过
		ConfigMgr::Init();  // 统一加载外部配置(Engine.json/Style.json...)

		// 窗口状态(大小/位置/最大化)来自引擎配置，支持重启恢复
		const auto& es = ConfigMgr::Get<EngineSettings>();
		WindowProps winProps(es.WindowWidth, es.WindowHeight, es.WindowTitle);
		winProps.PosX = es.WindowPosX;
		winProps.PosY = es.WindowPosY;
		winProps.Maximized = es.Maximized;
		winProps.VSync = es.VSync;
		m_Window = AppWindow::Create(winProps);

		RHI::RHIDeviceDesc desc;
		desc.WindowHandle = m_Window->GetNativeWindow();

		RHI::RHIDevice::Init(desc);

		DM_CORE_ASSERT(RHI::RHIDevice::Get(), "{}", "Failed to create RHIDevice");

		m_Renderer		= CreateUPtr<TriangleRenderer>();
		m_Input			= CreateUPtr<Input>();
		m_Clock			= CreateUPtr<Clock>();
		m_LayerStack	= CreateUPtr<LayerStack>();
		m_GameInst		= CreateUPtr<GameInst>();

		Input::m_Inst = static_cast<Input*>(m_Input.get());
		static_cast<Input*>(m_Input.get())->SetNativeWindow(m_Window->GetNativeWindow());
		return;
	}

	void Engine::Run()
	{
		m_Running = true;
		while (m_Running)
		{
			try
			{
				OnUpdate(m_Clock->Tick());
				if (static_cast<Input*>(m_Input.get())->IsKeyPressed(KeyCode::Escape))Close();
			}
			catch (const std::exception&e)
			{
				std::cerr << e.what() << std::endl;
			}
		}
		delete m_Inst;
	}

	void Engine::Shutdown()
	{
		// 回写窗口状态(大小/位置/最大化/垂直同步)到引擎配置
		if (m_Window)
		{
			auto& es = ConfigMgr::Get<EngineSettings>();
			int w = 0, h = 0, x = 0, y = 0;
			m_Window->GetWindowSize(w, h);
			m_Window->GetWindowPos(x, y);
			es.WindowWidth  = w;
			es.WindowHeight = h;
			es.WindowPosX   = x;
			es.WindowPosY   = y;
			es.Maximized    = m_Window->IsMaximized();
			es.VSync        = m_Window->IsVSyncEnabled();
		}
		// 统一保存所有外部配置到 Config/
		ConfigMgr::Shutdown();

		m_LayerStack.reset();
		Input::m_Inst = nullptr;
		m_Input.reset();
		m_Window.reset();
		m_Clock.reset();
		m_Renderer.reset();
	}

	void Engine::OnFrameBegin()
	{
		glfwPollEvents();
		m_Input->BeginFrame();
	}

	void Engine::OnUpdate(float deltaTime)
	{
		OnFrameBegin();
		if (!m_Minimized)
		{
			m_Renderer->BeginFrame();

			m_Renderer->Update(deltaTime);

			m_LayerStack->Update(deltaTime);

			m_Renderer->EndFrame();

			//m_Window->Update(deltaTime);
		}
		OnFrameEnd();
	}

	void Engine::OnFrameEnd()
	{
		m_Input->EndFrame();
	}


	void Engine::HandleEvent(Event* const e)
	{

		if (e->GetResourceType() == EEventType::WindowResize)OnWindowResize(e);
		if (e->GetResourceType() == EEventType::WindowClose)Close();

		PassEvent(e);
	}

	void Engine::PassEvent(Event* const e)
	{
		for (auto it = m_LayerStack->begin(); it != m_LayerStack->end(); ++it)
		{
			if ((*it) == nullptr) continue;
			if (e->bHandled) break;
			(*it)->HandleEvent(e);
		}
	}

	void Engine::OnWindowResize(Event* const e)
	{
		const float w = e->GetData<WindowResize>()->size.x;
		const float h = e->GetData<WindowResize>()->size.y;
		if (w == 0 || h == 0)
		{
			m_Minimized = true;
			return;
		}
		m_Minimized = false;
		LOG_CORE_INFO("OnWindowResize");
	}
}
