#include "DMPCH.h"
#include "Engine.h"
#include <GLFW/glfw3.h>
#include "Core/EventSystem/EventMgr.h"
#include <Resource/AssetManager.h>
#include <Core/EventSystem/Event/WindowEvent.h>
#include <Core/Render/Renderer/Renderer.h>
#include <Platform/Input/Input.h>
#include "Config.h"
#include <Platform/Window/Window.h>
#include "Core/Log.h"
#include <EngineConfig.h>

namespace DM {

	Engine* Engine::s_Instance = nullptr;

	Engine::Engine()
		: m_Running(false)
		, m_Minimized(false)
	{
	}

	Engine::~Engine()
	{
		s_Instance = nullptr;
	}

	UPtr<Engine> Engine::Create(const EngineConfig& config)
	{
		auto engine = UPtr<Engine>(new Engine());
		s_Instance = engine.get();
		engine->Init(config);
		return engine;
	}

	void Engine::Init(const EngineConfig& config)
	{
		// 日志
		Log::Init();

		// 配置（Phase 2 暂保留静态访问，后续注入实例）
		Config::Init(config.ConfigFilePath);

		// 窗口
		WindowProps winProps(config.WindowWidth, config.WindowHeight, config.WindowTitle);
		m_Window = Window::Create(winProps);
		m_Window->SetRefreshCallback([this]() { this->OnWindowRefresh(); });

		// 输入（注入 native window 到具体实现）
		m_Input = UPtr<Input>(new Input());
		Input::s_Instance = static_cast<Input*>(m_Input.get());
		static_cast<Input*>(m_Input.get())->SetNativeWindow(m_Window->GetNativeWindow());

		// 新事件总线（后续逐步从 EventMgr 迁移）
		m_EventBus = UPtr<EventBus>(new EventBus());

		// 时钟
		m_Clock = UPtr<Clock>(new Clock());

		// 层栈
		m_LayerStack = UPtr<LayerStack>(new LayerStack());

		// 注册旧 EventMgr 路由（GLFW 回调经 EventMgr→Engine）
		EventMgr::GetInst()->Register<Event>([this](Event* const e) { this->PassEvent(e); });
		EventMgr::GetInst()->Register<WindowClose>([this](Event* const e) { this->Close(); LOG_CORE_INFO("CloseWindow"); });
		EventMgr::GetInst()->Register<WindowResize>([this](Event* const e) { this->OnWindowResize(e); });

		// 渲染器
		Renderer::Init();
		RenderCommand::SetViewport(config.WindowWidth, config.WindowHeight);

		// 资产管理器（Phase 2 暂保留单例，后续注入实例）
		AssetManager::GetInst()->LoadAsset();
	}

	void Engine::RunEditor()
	{
		Run();
	}

	void Engine::RunGame()
	{
		Run();
	}

	void Engine::Run()
	{
		m_Running = true;
		m_Clock->Tick();
		float CurrentTime = 0.f;
		float LastTime = 0.f;
		while (m_Running)
		{
			CurrentTime = (float)glfwGetTime();
			const float DeltaTime = CurrentTime - LastTime;
			LastTime = CurrentTime;
			OnUpdate(DeltaTime);
		}
		Shutdown();
	}

	void Engine::Shutdown()
	{
		m_LayerStack.reset();
		m_EventBus.reset();
		Input::s_Instance = nullptr;
		m_Input.reset();
		m_Window.reset();
		m_Clock.reset();
	}

	void Engine::OnUpdate(float deltaTime)
	{
		// 输入帧更新
		m_Input->BeginFrame();

		// 退出快捷键
		if (static_cast<Input*>(m_Input.get())->IsKeyPressed(Key::Escape))
			Close();

		glfwPollEvents();

		m_Clock->Tick();
		m_LayerStack->Update(deltaTime);

		if (!m_Minimized)
		{
			m_Window->Update(deltaTime);
		}

		m_Input->EndFrame();
	}

	void Engine::OnWindowRefresh()
	{
		if (!m_Minimized)
		{
			m_LayerStack->Update(0.0f);
			m_Window->Update(0.0f);
		}
	}

	void Engine::PassEvent(Event* const e)
	{
		for (auto it = m_LayerStack->begin(); it != m_LayerStack->end(); ++it)
		{
			if ((*it) == nullptr) continue;
			if (e->bHandled) break;
			(*it)->OnEvent(e);
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
		Renderer::OnWindowResize((uint32_t)w, (uint32_t)h);
	}

}
