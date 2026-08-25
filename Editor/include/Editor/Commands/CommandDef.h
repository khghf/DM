#pragma once
#include <functional>
#include <string>

namespace DM
{
	/// <summary>
	/// 编辑器命令元数据：一条命令 = 唯一 Id + 菜单路径 + 快捷键 + 可用谓词 + 执行动作。
	/// 菜单栏与快捷键统一从命令注册表驱动：新增功能只需注册一条命令，无需改动任何 UI 代码。
	/// </summary>
	struct EditorCommandSpec
	{
		std::string Id;           // 唯一标识，如 "World.Open"
		std::string MenuPath;     // 菜单路径(以 '/' 分层)，如 "File/Open World"；空 = 不进菜单
		std::string ShortcutText; // 快捷键文本(菜单显示用)，如 "Ctrl+O"；空 = 无快捷键
		std::function<bool()> IsEnabled = [] { return true; };
		std::function<void()> Execute;
	};
}
