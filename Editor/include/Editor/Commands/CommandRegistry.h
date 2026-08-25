#pragma once
#include "Editor/Commands/CommandDef.h"
#include <string>
#include <vector>

namespace DM
{
	/// <summary>
	/// 编辑器命令注册表：集中登记命令元数据(菜单/快捷键/动作)。
	/// MenuBarPanel 据此自动生成菜单，EditorShortcuts 据此绑定快捷键，
	/// </summary>
	class CommandRegistry
	{
	public:
		static CommandRegistry& Get();

		CommandRegistry& Add(const EditorCommandSpec& spec);
		CommandRegistry& Add(std::string id, std::string menuPath, std::string shortcutText,std::function<bool()> isEnabled, std::function<void()> execute);

		const std::vector<EditorCommandSpec>& GetAll() const { return m_Commands; }

		/// <summary>取菜单根路径下的命令(如 "File/")</summary>
		std::vector<const EditorCommandSpec*> GetByMenuPath(const std::string& root) const;

		/// <summary>按 Id 执行命令；未找到时返回 false</summary>
		bool Invoke(const std::string& id) const;

	private:
		CommandRegistry() = default;
		std::vector<EditorCommandSpec> m_Commands;
	};
}
