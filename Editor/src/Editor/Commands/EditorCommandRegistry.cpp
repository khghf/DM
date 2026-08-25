#include "Editor/Commands/EditorCommandRegistry.h"
#include "Editor/Commands/CommandRegistry.h"
#include "Editor/Commands/ICommand.h"
#include "Editor/Commands/CommandInvoker.h"
#include "Editor/Core/EditorApplication.h"
#include "Editor/Core/EditorContext.h"
#include "Editor/UI/Panels/StylePanel.h"
#include <Engine.h>

namespace DM
{
	void EditorCommandRegistry::RegisterAll()
	{
		// 集中注册编辑器命令元数据：菜单路径、快捷键、可用谓词、执行动作三合一。
		// 新增菜单/快捷键功能只需在此追加一条 Add(...)。
		CommandRegistry::Get()
			.Add("World.New", "File/New World", "",
				[] { return true; },
				[] { CommandInvoker::Invoke<CmdNewWorld>(); })
			.Add("World.Open", "File/Open World", "Ctrl+O",
				[] { return true; },
				[] { CommandInvoker::Invoke<CmdOpenWorld>(); })
			.Add("World.Save", "File/Save World", "Ctrl+S",
				[] { return EditorContext::Get()->GetActiveWorld() != nullptr; },
				[] { CommandInvoker::Invoke<CmdSaveWorld>(); })
			.Add("App.Exit", "File/Exit", "",
				[] { return true; },
				[] { Engine::Get()->Close(); })
			.Add("PlayMode.Toggle", "", "F5",
				[] { return EditorContext::Get()->GetActiveWorld() != nullptr; },
				[] {
					if (EditorContext::Get()->IsPlaying())	CommandInvoker::Invoke<CmdStopPlayMode>();
					else									CommandInvoker::Invoke<CmdPlayMode>();
						
				})
			.Add("Window.Style", "Window/Style Settings", "",
				[] { return true; },
				[] {
					if (auto* panel = Editor::Get()->GetPanel<StylePanel>())panel->m_Open = !panel->m_Open;
				});
	}
}
