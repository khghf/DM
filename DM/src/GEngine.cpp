#include"DMPCH.h"
#include"GEngine.h"
#include<GLFW/glfw3.h>
#include"Core/EventSystem/EventMgr.h"
#include<glad/glad.h>
#include<Resource/AssetManager.h>
#include<Core/EventSystem/Event/WindowEvent.h>
#include<Core/Render/Renderer/Renderer.h>
#include<Platform/Input/Input.h>
#include"Config.h"
namespace DM {
	GEngine* Engine = new GEngine();

	bool GEngine::bAlreadyInit = false;

	GEngine::GEngine()
	{

	}

	GEngine::~GEngine()
	{
	}
	void GEngine::Run()
	{
		OnStart();
		float CurrentTime =0.f;
		float LastTime = 0.f;
		while (bRunning)
		{
			CurrentTime = (float)glfwGetTime();
			const float& DeltaTime = CurrentTime - LastTime;
			LastTime = CurrentTime;
			Update(DeltaTime);
		}
		OnExit();
	}
	void GEngine::Init()
	{
		assert(!bAlreadyInit&&"init engine more than once");
		//初始化子系统
		Log::Init();

		m_Win = SPtr<Window>(Window::Create());

		// 设置窗口刷新回调，在窗口 resize 期间强制渲染，防止拖拽时画面不更新
		m_Win->SetRefreshCallback([this]() { this->OnWindowRefresh(); });

		Config::Init();
		m_LayerStack = std::unique_ptr<LayerStack>(new LayerStack());
		EventMgr::GetInst()->Register<Event>([this](Event* const e){this->PassEvent(e);});
		EventMgr::GetInst()->Register<WindowClose>([this](Event* const e){this->bRunning = false;LOG_CORE_INFO("CloseWindow");});
		EventMgr::GetInst()->Register<WindowResize>([this](Event* const e) {this->OnWindowResize(e); });
		Renderer::Init();
#ifdef PROFILE_SCOPE
		//m_LayerStack->PushOverLayer(TimeMeasurerLayer::GetInst());
#endif // PROFILE_SCOPE

		AssetManager::GetInst()->LoadAsset();
		bAlreadyInit = true;
	}
	void GEngine::OnStart()
	{
		glClearColor(0.2f, 0.2f, 0.2f, 1.f);
	}
	void GEngine::Update(float DeltaTime)
	{
		if (Input::s_Instance && Input::s_Instance->IsKeyPressed(Key::Escape))Close();
		glfwPollEvents();
		m_LayerStack->Update(DeltaTime);
		if (!bMinimized)
		{
			m_Win->Update(DeltaTime);
		}
	}
	void GEngine::OnExit()
	{
	}
	void GEngine::PassEvent(Event* const e)
	{
		for (auto  it= m_LayerStack->begin();it!=m_LayerStack->end();++it)
		{
			if ((*it) == nullptr)continue;
			if (e->bHandled)break;
			(*it)->OnEvent(e);
		}
	}
	void GEngine::OnWindowResize(Event* const e)
	{
		const float w = e->GetData<WindowResize>()->size.x;
		const float h = e->GetData<WindowResize>()->size.y;
		if (w == 0 || h == 0)
		{
			bMinimized = true;
			return;
		}
		bMinimized = false;
		LOG_CORE_INFO("OnWindowResize");
		Renderer::OnWindowResize((uint32_t)w, (uint32_t)h);
	}

	void GEngine::OnWindowRefresh()
	{
		// 在窗口 resize/move 期间被 Windows WM_PAINT 触发，强制执行一次渲染+交换
		if (!bMinimized)
		{
			m_LayerStack->Update(0.0f);
			m_Win->Update(0.0f);
		}
	}
}
