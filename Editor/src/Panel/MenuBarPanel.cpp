#include <DMPCH.h>
#include "Panel/MenuBarPanel.h"
#include <imgui.h>
#include <Foundation/Util/PlatformUtils.h>
#include "DM.h"
#include<Core/AssetManagent/AssetMgr.h>
#include<Editor.h>
#include<Panel/ContentBrowserPanel.h>
#include<Foundation/FileSystem.h>
#include<EditorCommand/CommandInvoker.h>
namespace DM
{
	MenuBarPanel::MenuBarPanel()
	{
	}

	void MenuBarPanel::Render()
	{
		if (ImGui::BeginMenuBar())
		{
			RenderFileMenu();
			ImGui::EndMenuBar();
		}
	}

	void MenuBarPanel::RenderFileMenu()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("NewWorld"))
			{
				CommandInvoker::Invoke<CmdNewWorld>();
			}
			if (ImGui::MenuItem("OpenWorld", "Ctrl+O"))
			{
				std::string path = FileDialog::OpenFile();
				if (!path.empty())OpenWorld(path);
			}
			if (ImGui::MenuItem("SaveScene", "Ctrl+S", nullptr, m_Context.get() != nullptr))
			{
					
			}
			if (ImGui::MenuItem("Exit"))
			{
				Engine::Get()->Close();
			}
			ImGui::EndMenu();
		}
	}
	
	void MenuBarPanel::OpenWorld(const std::string& path)
	{

	}
}
