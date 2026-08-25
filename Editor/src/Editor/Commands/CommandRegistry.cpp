#include "Editor/Commands/CommandRegistry.h"
#include <utility>

namespace DM
{
	CommandRegistry& CommandRegistry::Get()
	{
		static CommandRegistry inst;
		return inst;
	}

	CommandRegistry& CommandRegistry::Add(const EditorCommandSpec& spec)
	{
		m_Commands.push_back(spec);
		return *this;
	}

	CommandRegistry& CommandRegistry::Add(std::string id, std::string menuPath, std::string shortcutText,std::function<bool()> isEnabled, std::function<void()> execute)
	{
		m_Commands.push_back(EditorCommandSpec{ std::move(id), std::move(menuPath), std::move(shortcutText),std::move(isEnabled), std::move(execute) });
		return *this;
	}

	std::vector<const EditorCommandSpec*> CommandRegistry::GetByMenuPath(const std::string& root) const
	{
		std::vector<const EditorCommandSpec*> result;
		for (const auto& cmd : m_Commands)
		{
			if (cmd.MenuPath.rfind(root, 0) == 0)
			{
				result.push_back(&cmd);
			}
		}
		return result;
	}

	bool CommandRegistry::Invoke(const std::string& id) const
	{
		for (const auto& cmd : m_Commands)
		{
			if (cmd.Id == id)
			{
				if (cmd.Execute) cmd.Execute();
				return true;
			}
		}
		return false;
	}
}
