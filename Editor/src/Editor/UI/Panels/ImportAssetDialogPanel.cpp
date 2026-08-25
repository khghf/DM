#include "Editor/UI/Panels/ImportAssetDialogPanel.h"
#include <Editor/Core/EditorContext.h>
#include <Editor/UI/EditorUI.h>
#include <Core/Log.h>
namespace DM
{
	void ImportAssetDialogPanel::ShowNewAssets(std::vector<AssetScanner::NewAsset> assets)
	{
		if (assets.empty()) return;
		m_NewAssets = std::move(assets);
	}

	void ImportAssetDialogPanel::Render()
	{
		if (m_NewAssets.empty()) return;

		EditorUI::OpenPopup("Import New Assets");
		if (EditorUI::BeginPopupModal("Import New Assets", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			EditorUI::TextUnformatted("Discovered new importable assets:");

			EditorUI::Separator();
			const float listHeight = EditorUI::GetTextLineHeightWithSpacing() * std::min<size_t>(m_NewAssets.size(), 10) + 8.0f;
			if (EditorUI::BeginChild("##assetlist", ImVec2(480.0f, listHeight), true))
			{
				for (const auto& asset : m_NewAssets)
				{
					EditorUI::BulletText("%s", asset.FileName.c_str());
					EditorUI::TextDisabled("    %s", asset.SourceFilePath.c_str());
				}
			}
			EditorUI::EndChild();
			EditorUI::Separator();

			if (!m_bImporting)
			{
				if (EditorUI::Button("Import All"))
				{
					ImportAll();
				}
				EditorUI::SameLine();
				if (EditorUI::Button("Cancel"))
				{
					m_NewAssets.clear();
					EditorUI::CloseCurrentPopup();
				}
			}
			else
			{
				EditorUI::TextUnformatted("Importing...");
			}

			EditorUI::EndPopup();
		}
	}

	void ImportAssetDialogPanel::ImportAll()
	{
		m_bImporting = true;
		for (const auto& asset : m_NewAssets)
		{
			if (EditorContext::GetService<AssetScanner>()->Import(asset.SourceFilePath))
			{
				LOG_CORE_INFO("Imported asset: {}", asset.SourceFilePath);
			}
		}
		m_NewAssets.clear();
		m_bImporting = false;
		EditorUI::CloseCurrentPopup();
	}
}
