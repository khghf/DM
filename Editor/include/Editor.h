#pragma once
#include<Core/Layer/Layer.h>
#include<Core/AssetManagent/AssetMgr.h>
#include<filesystem>
#include<Defines.h>
#include"Panel/ViewportPanel.h"
#include"Panel/MenuBarPanel.h"
#include<vector>
namespace DM
{
	class ImGuiRenderer;
	class World;
	class Panel;
	class EDITOR_API Editor :public Layer
	{
		friend class APP;

		Editor(const std::string_view& name);
	public:
		virtual ~Editor();
		virtual void OnAttach()override;
		
		virtual void OnDetach()override;
		
		virtual void OnUpdate(float DeltaTime)override;
		
		virtual void HandleEvent(DM::Event* const e)override;

		static Editor* Get()
		{
			static Editor* inst = new Editor("Editor");
			return inst;
		}

		Panel* GetContentBrowser()const { return m_Panels[m_ContentBrowserIndex]; }

	protected:
		void Begin();
		void Render();
		void End();


		template<typename T,typename...Args>
		int AddPanel(Args&&...arg) { m_Panels.emplace_back(new T{std::forward<Args>(arg)...}); return m_Panels.size() - 1; }
	
	public:
		void ChangeEditorWorld(SPtr<World>world);
		SPtr<World> GetActiveWorld()const { return m_ActiveWorld; }
	private:
		void OnSceneChanged();
		void OpenScene(std::filesystem::path p);
	private:
		std::vector<Panel*>m_Panels;

		int m_ViewPortIndex=-1;
		int m_MenuBarIndex=-1;
		int m_SceneHierarchyIndex=-1;
		int m_ContentBrowserIndex=-1;

		UPtr<ImGuiRenderer>m_ImGuiRenderer;

		SPtr<World>m_EditorWorld;
		SPtr<World>m_PlayWorld;
		SPtr<World>m_ActiveWorld;

		bool m_IsPlaying = false;
	};
}
