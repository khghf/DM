#pragma once
#include"Panel.h"
#include"Framework/Base/Entity.h"
namespace DM
{
	class SceneHierarchyPanel:public Panel
	{
		friend class Editor;
		SceneHierarchyPanel() { };
	protected:
		virtual void Render()override;
		void DrawEntityNode(const Entity& entity);
		void DrawComponents(const Entity& entity);
	private:
		Entity m_SelectedEntity;
	};
}


