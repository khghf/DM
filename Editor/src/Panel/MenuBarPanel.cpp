#include <DMPCH.h>
#include "Panel/MenuBarPanel.h"
#include <imgui.h>
#include <Tool/Util/PlatformUtils.h>
#include "DM.h"

namespace DM
{
	MenuBarPanel::MenuBarPanel(const SPtr<World>& context)
	{
		SetContext(context);
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
			if (ImGui::MenuItem("NewScene"))
			{
				if (OnNewScene)
					OnNewScene();
			}
			if (ImGui::MenuItem("OpenScene", "Ctrl+O"))
			{
				std::string path = FileDialog::OpenFile();
				if (OnOpenScene && !path.empty())
					OnOpenScene(path);
			}
			if (ImGui::MenuItem("SaveScene", "Ctrl+S", nullptr, m_Context.get() != nullptr))
			{
				if (OnSaveScene)
					OnSaveScene();
			}
			if (ImGui::MenuItem("Exit"))
			{
				Engine::Get().Close();
			}
			ImGui::EndMenu();
		}
	}
}
