#include<DMPCH.h>
#include "Panel/ContentBrowserPanel.h"
#include <imgui.h>
#include<Config.h>
#include<Platform/Input/Input.h>
#include<Core/Render/Texture/Texture.h>
#include<Tool/Util/GameStatic.h>
namespace DM
{
	namespace fs = std::filesystem;
	struct Icons
	{
		SPtr<Texture2D>File;
		SPtr<Texture2D>Folder;
	};
	static Icons icon;


	ContentBrowserPanel::ContentBrowserPanel(const SPtr<World>& context)
	{
		 SetContext(context);
		 CurPath = fs::path(Config::AssetFolder());
		 CurPath = fs::path(Config::AssetFolder());
		 icon.File = GameStatic::GetTexture2D("File");
		 icon.Folder = GameStatic::GetTexture2D("Folder");
	}

	void ContentBrowserPanel::Render()
	{
		ImGui::Begin("Content Browser");
		{
			static float padding = 16.f; 
			static float tumbnailSize = 128.f;
			static float cellSize = tumbnailSize+ padding;
			static bool isDirectory = false;
			float panelWidth = ImGui::GetContentRegionAvail().x;
			int colCount = (int)(panelWidth / cellSize);
			colCount = colCount > 0 ? colCount : 1;
			if (CurPath != fs::path(Config::AssetFolder()))
			{
				if (ImGui::Button("<-"))
				{
					CurPath = CurPath.parent_path();
				}
				else if (ImGui::IsKeyReleased(ImGuiKey_MouseX1) && ImGui::IsWindowHovered())
				{
					CurPath = CurPath.parent_path();
				}
			}
			ImGui::Columns(colCount, nullptr, false);
			for (auto& it : fs::directory_iterator(CurPath))
			{
				std::string fileName = it.path().filename().string();
				isDirectory = it.is_directory();
				SPtr<Texture2D>img = isDirectory ? icon.Folder : icon.File;
				ImGui::ImageButton(fileName.c_str(), (ImTextureRef)img->GetId(), { tumbnailSize,tumbnailSize }, { 0,1 }, { 1,0 });
				
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if(isDirectory)CurPath /= fileName;
				}
				if (ImGui::BeginDragDropSource())
				{
					const wchar_t* path = it.path().c_str();
					const size_t size = std::wstring(path).size();
					ImGui::SetDragDropPayload("CONTENT_BROWSER_DRAG_ITEM", path,(size+1)*sizeof(wchar_t));
					ImGui::EndDragDropSource();
				}
				ImGui::TextWrapped(fileName.c_str());
				ImGui::NextColumn();
			}
			
		}
		ImGui::Columns(1);
		ImGui::End();
	}
}
