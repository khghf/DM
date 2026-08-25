#include <DMPCH.h>
#include "Editor/UI/Panels/MenuBarPanel.h"
#include <Editor/UI/EditorUI.h>
#include "DM.h"
#include<Core/AssetManagent/AssetMgr.h>
#include<Editor/Core/EditorContext.h>
#include<Editor/UI/Panels/ContentBrowserPanel.h>
#include<Editor/Commands/CommandRegistry.h>
#include<Editor/Commands/CommandInvoker.h>

namespace DM
{
	namespace
	{
		/// <summary>从菜单路径取末段作为菜单项显示文本("File/Open World" -> "Open World")</summary>
		std::string MenuLabel(const std::string& menuPath)
		{
			const size_t slash = menuPath.rfind('/');
			return slash == std::string::npos ? menuPath : menuPath.substr(slash + 1);
		}
	}

	MenuBarPanel::MenuBarPanel()
	{
	}

	void MenuBarPanel::Render()
	{
		if (EditorUI::BeginMenuBar())
		{
			// 播放控制(工具栏最前)
			if (EditorContext::Get()->IsPlaying())
			{
				if (EditorUI::MenuItem("Stop"))
				{
					CommandInvoker::Invoke<CmdStopPlayMode>();
				}
			}
			else
			{
				if (EditorUI::MenuItem("Play", nullptr, false, EditorContext::Get()->GetActiveWorld() != nullptr))
				{
					CommandInvoker::Invoke<CmdPlayMode>();
				}
			}

			// 菜单项由命令注册表自动生成
			RenderMenuRoot("File");
			RenderMenuRoot("Window");
			EditorUI::EndMenuBar();
		}
	}

	void MenuBarPanel::RenderMenuRoot(const std::string& root)
	{
		if (EditorUI::BeginMenu(root.c_str()))
		{
			for (const EditorCommandSpec* cmd : CommandRegistry::Get().GetByMenuPath(root + "/"))
			{
				if (!cmd) continue;
				const bool enabled = cmd->IsEnabled ? cmd->IsEnabled() : true;
				if (EditorUI::MenuItem(MenuLabel(cmd->MenuPath).c_str(),
					cmd->ShortcutText.empty() ? nullptr : cmd->ShortcutText.c_str(),
					nullptr, enabled))
				{
					if (cmd->Execute) cmd->Execute();
				}
			}
			EditorUI::EndMenu();
		}
	}
}
