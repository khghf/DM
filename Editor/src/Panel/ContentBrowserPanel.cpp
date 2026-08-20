#include<DMPCH.h>
#include "Panel/ContentBrowserPanel.h"
#include <imgui.h>
#include<Config.h>
#include<Platform/Input/Input.h>
#include<Core/AssetManagent/AssetMgr.h>

#ifdef ENABLE_VULKAN_API
#include<backends/imgui_impl_vulkan.h>
#include<Core/RHI/Backend/Vulkan/VulkanTexture.h>
#endif // ENABLE_VULKAN_API


namespace DM
{
	namespace fs = std::filesystem;
	struct Icon
	{
		SPtr<Texture2D> Texture;
#ifdef ENABLE_VULKAN_API
		VkDescriptorSet	id;
#endif // ENABLE_VULKAN_API
	};
	struct Icons
	{
		Icon File;
		Icon Folder;
	};
	static Icons icon;

	static void AddTexture(Icon& icon)
	{
		RHI::VulkanTexture* vTexture = static_cast<RHI::VulkanTexture*>(icon.Texture->GetRHIResource());
		icon.id = ImGui_ImplVulkan_AddTexture(vTexture->GetvkImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
	static void RemoveTexture(Icon& icon)
	{

#ifdef ENABLE_VULKAN_API
		if (icon.id != VK_NULL_HANDLE)
		{
			ImGui_ImplVulkan_RemoveTexture(icon.id);
			icon.id = VK_NULL_HANDLE;
		}
#endif // ENABLE_VULKAN_API
		
		icon.Texture.reset();
	}



	ContentBrowserPanel::ContentBrowserPanel()
	{
		return;
		CurPath = fs::path(AssetRootDir);
		 icon.File.Texture = AssetMgr::LoadAsset<Texture2D>(AssetRootDir + "/Icon/File.png");
		 icon.Folder.Texture = AssetMgr::LoadAsset<Texture2D>(AssetRootDir + "/Icon/Folder.png");

		 AddTexture(icon.File);
		 AddTexture(icon.Folder);
	}

	ContentBrowserPanel::~ContentBrowserPanel()
	{
		return;
		RemoveTexture(icon.File);
		RemoveTexture(icon.Folder);
	}

	void ContentBrowserPanel::Render()
	{
		return;
		ImGui::Begin("Content Browser");
		{
			static float padding = 16.f; 
			static float tumbnailSize = 128.f;

			static float cellSize = tumbnailSize+ padding;


			float panelWidth = ImGui::GetContentRegionAvail().x;

			int colCount = (int)(panelWidth / cellSize);

			colCount = colCount > 0 ? colCount : 1;

			if (CurPath != fs::path(AssetRootDir))
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

				static bool isDirectory = false;

				isDirectory = it.is_directory();

				const auto&img = isDirectory ? icon.Folder : icon.File;
				ImGui::ImageButton(fileName.c_str(), (ImTextureRef)img.id, { tumbnailSize,tumbnailSize }, { 0,1 }, { 1,0 });
				
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if(isDirectory)CurPath /= fileName;
				}

				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					std::string path = it.path().string();
					//const size_t size = std::wstring(path).size();
					ImGui::SetDragDropPayload("CONTENT_BROWSER_DRAG_ITEM", path.data(), (path.size()+1)*sizeof(char));
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
