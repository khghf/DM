#include "EditorLayer.h"
#include<imgui.h>
#include"DM.h"
#include"Panel/SceneHierarchyPanel.h"
#include"Panel/ContentBrowserPanel.h"
#include<Tool/Util/PlatformUtils.h>
#include"Config.h"
#include<EditorStyle.h>
#include<ImGui/imgui_impl_glfw.h>
#include<ImGui/imgui_impl_opengl3.h>
#include <Engine.h>
#include<Core/Render/Renderer/Renderer2D.h>
namespace DM
{
	//用于延迟初始化imgui
	static bool bImGuiInitialized = false;
	static void InitImGui()
	{
		if (bImGuiInitialized)return;
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		ImGui_ImplOpenGL3_Init("#version 460");
		ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(Engine::Get().GetWindow().GetNativeWindow()), true);

		EditorStyle::SetDefaultStyle();

		bImGuiInitialized = true;
	}
	static void ShutDownImGui()
	{
		if (!bImGuiInitialized)return;
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	static void BeginDockSpace()
	{
		ImGuiIO& io = ImGui::GetIO();
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		bool opt_opendock = true;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)window_flags |= ImGuiWindowFlags_NoBackground;
		if (!opt_padding)ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &opt_opendock, window_flags);
		if (!opt_padding)ImGui::PopStyleVar();
		if (opt_fullscreen)ImGui::PopStyleVar(2);
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
	}
	static void EndDockSpace()
	{
		ImGui::End();
	}


	EditorLayer::EditorLayer(const std::string_view& name) :Layer(name)
	{
		
	}

	EditorLayer::~EditorLayer()
	{
		
	}

	void EditorLayer::OnAttach()
	{
		__super::OnAttach();
		InitImGui();
		Renderer2D::Init();

		float x = (float)Engine::Get().GetWindow().GetWidth();
		float y = (float)Engine::Get().GetWindow().GetHeight();

		// 创建 ViewportPanel，负责场景渲染和视口显示
		m_ViewportPanel = CreateSPtr<ViewportPanel>(new ViewportPanel(m_ActiveScene, x, y));
		m_ViewportPanel->OnOpenScene = [this](std::filesystem::path path) {
			OpenScene(path);
		};

		// 创建 MenuBarPanel，负责菜单栏渲染
		m_MenuBarPanel = CreateSPtr<MenuBarPanel>(new MenuBarPanel(m_ActiveScene));
		m_MenuBarPanel->OnNewScene = [this]() {
			if (m_ActiveScene)
			{
				Serializer::Serialize(m_ActiveScene);
			}
			m_ActiveScene.reset(new World());
			OnSceneChanged();
		};
		m_MenuBarPanel->OnOpenScene = [this](std::filesystem::path path) {
			OpenScene(path);
		};
		m_MenuBarPanel->OnSaveScene = [this]() {
			if (!m_ActiveScene) return;
			std::string path = FileDialog::SaveFile(m_ActiveScene->m_Name.c_str());
			if (!path.empty())
			{
				m_ActiveScene->m_path = path + "." + m_ActiveScene->s_FileExtension.data();
				Serializer::Serialize(m_ActiveScene);
			}
		};

		m_HierarchyPanel = CreateSPtr<SceneHierarchyPanel>(new SceneHierarchyPanel(m_ActiveScene));
		m_ContentPanel = CreateSPtr<ContentBrowserPanel>(new ContentBrowserPanel(m_ActiveScene));
	}

	void EditorLayer::OnDetach()
	{
		__super::OnDetach();
		Renderer2D::ShoutDown();
		if (m_ActiveScene)
		{
			Serializer::Serialize(m_ActiveScene);
		}
	}

	void EditorLayer::OnUpdate(float DeltaTime)
	{
		m_ViewportPanel->OnUpdate(DeltaTime);
	}

	void EditorLayer::OnEvent(Event* const e)
	{
		m_ViewportPanel->OnEvent(e);
	}

	void EditorLayer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void EditorLayer::Render()
	{
		BeginDockSpace();
		//@todo 待移除
		ImGui::ShowDemoWindow();

		{
			m_MenuBarPanel->Render();//渲染菜单栏
			m_ViewportPanel->Render();//渲染视口 + Setting
			m_HierarchyPanel->Render();//渲染层级面板
			m_ContentPanel->Render();
		}
		EndDockSpace();
	}

	void EditorLayer::End()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void EditorLayer::UpdateEditor()
	{

	}

	void EditorLayer::UpdateRunTime()
	{

	}

	void EditorLayer::OnSceneChanged()
	{
		m_ViewportPanel->SetContext(m_ActiveScene);
		m_HierarchyPanel->SetContext(m_ActiveScene);
		m_ContentPanel->SetContext(m_ActiveScene);
	}

	void EditorLayer::OpenScene(std::filesystem::path p)
	{
		
		if (std::filesystem::is_regular_file(p)&&Util::HasSuffix(p.string(),World::s_FileExtension.data()))
		{
			if (m_ActiveScene)
			{
				Serializer::Serialize(m_ActiveScene);
				m_ActiveScene.reset();
			}
			m_ActiveScene=Serializer::DeSerialize(p.string());
			DM_CORE_ASSERT(m_ActiveScene, "Open scene failed path:{}", p.string());
			OnSceneChanged();
			LOG_CORE_INFO("OpenScene:{}", p.string());
		}
	}

}
