#pragma once

namespace DM
{
	/// <summary>
	/// 编辑器命令集中注册入口：在 Editor::OnAttach 调用一次，
	/// 集中登记所有菜单命令的元数据，菜单栏与快捷键自动生效。
	/// </summary>
	class EditorCommandRegistry
	{
	public:
		static void RegisterAll();
	};
}
