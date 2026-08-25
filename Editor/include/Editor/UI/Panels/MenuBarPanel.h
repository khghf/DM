#pragma once
#include "Panel.h"
#include <functional>
#include <string>

namespace DM
{
	/// <summary>
	/// 主菜单栏面板：菜单项全部由命令注册表自动生成(按 MenuPath 分层)，
	/// 新增菜单功能只需在 EditorCommandRegistry::RegisterAll() 注册命令。
	/// </summary>
	class MenuBarPanel : public Panel
	{
		friend class Editor;
		MenuBarPanel();
	public:
	protected:
		virtual void Render() override;
		/// <summary>按菜单根路径(如 "File")自动渲染其下的注册命令</summary>
		void RenderMenuRoot(const std::string& root);
	};
}
