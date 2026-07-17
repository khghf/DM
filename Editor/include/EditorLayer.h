#pragma once
#include<Core/Layer/Layer.h>
#include<Resource/AssetManager.h>
#include"Core/Serializer.h"
#include<filesystem>
#include<Defines.h>
#include"Panel/ViewportPanel.h"
#include"Panel/MenuBarPanel.h"

namespace DM
{
	class World;
	class Panel;
	struct DockSpace
	{
		
	};
	
	class EDITOR_API EditorLayer :public Layer
	{
	public:
		EditorLayer(const std::string_view& name);
		virtual ~EditorLayer();
		virtual void OnAttach()override;
		
		virtual void OnDetach()override;
		
		virtual void OnUpdate(float DeltaTime)override;
		
		virtual void OnEvent(DM::Event* const e)override;

	protected:
		virtual void Begin()override;
		virtual void Render()override;
		virtual void End()override;

		void UpdateEditor();
		void UpdateRunTime();
	private:
		void OnSceneChanged();
		void OpenScene(std::filesystem::path p);
	private:
		SPtr<World> m_ActiveScene;
		SPtr<Panel> m_HierarchyPanel;
		SPtr<Panel> m_ContentPanel;
		SPtr<ViewportPanel> m_ViewportPanel;
		SPtr<MenuBarPanel> m_MenuBarPanel;
		/*enum class SceneState:uint8_t
		{

		};*/
	};


}
