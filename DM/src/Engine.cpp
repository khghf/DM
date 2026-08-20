#include "DMPCH.h"
#include "Engine.h"
#include <GLFW/glfw3.h>
#include <Core/EventBus/Event/WindowEvent.h>
#include <Platform/Input/Input.h>
#include "Config.h"
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

		Config::Init(config.ConfigFilePath);

		WindowProps winProps(config.WindowWidth, config.WindowHeight, config.WindowTitle);
		m_Window = AppWindow::Create(winProps);

		RHI::RHIDeviceDesc desc;
		desc.WindowHandle = m_Window->GetNativeWindow();
		RHI::RHIDevice::Init(desc);

		DM_CORE_ASSERT(RHI::RHIDevice::Get(), "{}", "Failed to create RHIDevice");

		m_Renderer = CreateUPtr<TriangleRenderer>();


		m_Input = UPtr<Input>(new Input());
		Input::m_Inst = static_cast<Input*>(m_Input.get());
		static_cast<Input*>(m_Input.get())->SetNativeWindow(m_Window->GetNativeWindow());
		m_Clock = UPtr<Clock>(new Clock());
		m_LayerStack = UPtr<LayerStack>(new LayerStack());
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

			m_Renderer->UpdateData(deltaTime);

			m_LayerStack->UpdateData(deltaTime);

			m_Renderer->EndFrame();

			//m_Window->UpdateData(deltaTime);
		}
		OnFrameEnd();
	}

	void Engine::OnFrameEnd()
	{
		m_Input->EndFrame();
	}


	void Engine::HandleEvent(Event* const e)
	{

		if (e->GetAssetType() == EEventType::WindowResize)OnWindowResize(e);
		if (e->GetAssetType() == EEventType::WindowClose)Close();


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
