#pragma once
#include "Panel.h"
#include <functional>
#include <filesystem>

namespace DM
{
	class MenuBarPanel : public Panel
	{
		friend class EditorLayer;
		MenuBarPanel(const SPtr<World>& context);
	public:
		// 回调：由 EditorLayer 设置，用于处理菜单操作对编辑器状态的修改
		std::function<void()> OnNewScene;
		std::function<void(std::filesystem::path)> OnOpenScene;
		std::function<void()> OnSaveScene;
	protected:
		virtual void Render() override;
		void RenderFileMenu();
	};
}
