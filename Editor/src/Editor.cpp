#include "Editor.h"
#include <Engine.h>
#include<DM.h>
#include<imgui.h>
#include"Panel/SceneHierarchyPanel.h"
#include"Panel/ContentBrowserPanel.h"
#include<Foundation/Util/PlatformUtils.h>
#include"Config.h"
#include<EditorStyle.h>
#include<ImGuiBackend/ImGuiRenderer.h>
#include<ImGuiBackend/ImGuiInitializer.h>
#include<EditorCommand/CommandHistory.h>
#include<ranges>
namespace DM
{
	Editor::Editor(const std::string_view& name) :Layer(name)
	{
		
	}

	Editor::~Editor()
	{
		
	}

	void Editor::OnAttach()
	{
		m_ImGuiRenderer = CreateUPtr<ImGuiRenderer>();
		ImGuiInitializer::Init(m_ImGuiRenderer.get());

		m_ViewPortIndex=AddPanel<ViewportPanel>();
		m_MenuBarIndex= AddPanel<MenuBarPanel>();
		m_SceneHierarchyIndex=AddPanel<SceneHierarchyPanel>();
		m_ContentBrowserIndex=AddPanel<ContentBrowserPanel>();
	
		std::ranges::for_each(m_Panels, [&](Panel* panel) {panel->SetContext(m_ActiveWorld); });
	}

	void Editor::OnDetach()
	{

		RHI::RHIDevice::Get()->WaitGPUIdle();

		for (Panel* panel : m_Panels)
		{
			if (panel)delete panel;

		}
		ImGuiInitializer::ShutDown();
	}

	void Editor::OnUpdate(float DeltaTime)
	{
		Begin();
		Render();
		End();
	}

	void Editor::HandleEvent(Event* const e)
	{
		std::ranges::for_each(m_Panels, [&](Panel* panel) {panel->HandleEvent(e); });
	}

	void Editor::Begin()
	{
		m_ImGuiRenderer->Begin();
		if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Z, ImGuiInputFlags_RouteGlobal))
		{
			CommandHistory::Get()->Undo();
		}
		if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Y, ImGuiInputFlags_RouteGlobal))
		{
			CommandHistory::Get()->Redo();
		}
	}

	void Editor::Render()
	{
		//@todo 待移除
		ImGui::ShowDemoWindow();

		for (Panel* panel : m_Panels)
		{
			panel->Render();
		}
	}

	void Editor::End()
	{
		m_ImGuiRenderer->End();
	}


	void Editor::ChangeEditorWorld(SPtr<World> world)
	{
		if (m_IsPlaying)return;
		m_EditorWorld = world;
		m_ActiveWorld = world;
		std::ranges::for_each(m_Panels, [&](Panel* panel) {panel->SetContext(m_ActiveWorld); });
	}

	void Editor::OnSceneChanged()
	{
		/*m_ViewportPanel->SetContext(m_ActiveScene);
		m_HierarchyPanel->SetContext(m_ActiveScene);
		m_ContentPanel->SetContext(m_ActiveScene);*/
	}

	void Editor::OpenScene(std::filesystem::path p)
	{
		
		/*if (std::filesystem::is_regular_file(p)&&Util::HasSuffix(p.string(),World::s_FileExtension.data()))
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
		}*/
	}

}
