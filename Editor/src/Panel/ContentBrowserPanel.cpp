// ContentBrowserPanel.cpp
#include <DMPCH.h>
#include "Panel/ContentBrowserPanel.h"
#include <imgui.h>
#include <Config.h>
#include <Platform/Input/Input.h>
#include <Core/AssetManagent/AssetMgr.h>
#include <Core/AssetManagent/AssetMetaDatabase.h>
#include <map>

#ifdef ENABLE_VULKAN_API
#include <backends/imgui_impl_vulkan.h>
#include <Core/RHI/Backend/Vulkan/VulkanTexture.h>
#endif

namespace DM
{
    namespace fs = std::filesystem;

    struct Icon
    {
        SPtr<Texture2D> Texture;
#ifdef ENABLE_VULKAN_API
        VkDescriptorSet id;
#endif
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
#endif
        icon.Texture.reset();
    }

    ContentBrowserPanel::ContentBrowserPanel()
    {
        CurPath = fs::path(AssetRootDir);
        icon.File.Texture = AssetMgr::LoadAsset<Texture2D>(AssetRootDir + "/Icon/File.png");
        icon.Folder.Texture = AssetMgr::LoadAsset<Texture2D>(AssetRootDir + "/Icon/Folder.png");

        AddTexture(icon.File);
        AddTexture(icon.Folder);
    }

    ContentBrowserPanel::~ContentBrowserPanel()
    {
        RemoveTexture(icon.File);
        RemoveTexture(icon.Folder);
    }

    void ContentBrowserPanel::RefreshDirectoryCache()
    {
        auto database = AssetMetaDatabase::Get();

        // 感知数据库变更：修改计数变化则强制刷新，保证新增/删除资源及时显示
        uint64_t modifyCount = database->GetModifyCount();
        if (modifyCount != m_LastModifyCount)
        {
            m_LastModifyCount = modifyCount;
            m_NeedRefresh = true;
        }
        if (!m_NeedRefresh) return;

        m_Cache.clear();

        fs::path curPath = database->NormalizePath(CurPath.string());

        // 源文件路径 -> 文件条目(按源路径去重，兼容数据库中同路径的重复历史记录)
        std::map<std::string, CachedEntry> fileEntries;
        // 子目录名->完整路径(去重，仅包含已注册资源的子目录)
        std::map<std::string, fs::path> subDirs;

        // 以 AssetMetaDatabase 为唯一数据源：遍历全部已注册资源，映射到当前目录
        for (const auto& [guid, record] : database->GetAllRecords())
        {
            fs::path sourcePath = database->NormalizePath(record.SourceFilePath);
            fs::path parent = sourcePath.parent_path();

            if (parent == curPath)
            {
                // 直接位于当前目录下的注册资源
                CachedEntry entry;
                entry.path = sourcePath;
                entry.fileName = sourcePath.filename().string();
                entry.isDirectory = false;
                entry.assetId = guid;
                // emplace 仅在 key 不存在时插入，重复记录只保留第一条
                fileEntries.emplace(sourcePath.string(), std::move(entry));
            }
            else
            {
                // 位于当前目录子目录中的资源：仅记录第一级子目录
                fs::path rel = sourcePath.lexically_relative(curPath);
                if (rel.empty() || *rel.begin() == ".." || *rel.begin() == ".") continue;

                std::string subDirName = (*rel.begin()).string();
                subDirs[subDirName] = curPath / subDirName;
            }
        }

        // 添加去重后的文件项
        for (auto& [sourcePath, entry] : fileEntries)
        {
            m_Cache.push_back(std::move(entry));
        }

        // 添加子目录项(只显示包含已注册资源的目录)
        for (const auto& [name, dirPath] : subDirs)
        {
            CachedEntry entry;
            entry.path = dirPath;
            entry.fileName = name;
            entry.isDirectory = true;
            m_Cache.push_back(std::move(entry));
        }

        // 目录优先，其次按名称排序
        std::sort(m_Cache.begin(), m_Cache.end(), [](const CachedEntry& a, const CachedEntry& b) 
            {
            if (a.isDirectory != b.isDirectory)
                return a.isDirectory > b.isDirectory;  // 目录在前
            return a.fileName < b.fileName;            // 按名称排序
            });

        m_NeedRefresh = false;
    }

    void ContentBrowserPanel::Render()
    {
        ImGui::Begin("Content Browser");
        {
            static float padding = 16.f;
            static float thumbnailSize = 128.f;
            static float cellSize = thumbnailSize + padding;

            float panelWidth = ImGui::GetContentRegionAvail().x;
            int colCount = (int)(panelWidth / cellSize);
            colCount = colCount > 0 ? colCount : 1;

            // 返回按钮
            if (CurPath != fs::path(AssetRootDir))
            {
                if (ImGui::Button("<-"))
                {
                    CurPath = CurPath.parent_path();
                    m_NeedRefresh = true;
                }
                else if (ImGui::IsKeyReleased(ImGuiKey_MouseX1) && ImGui::IsWindowHovered())
                {
                    CurPath = CurPath.parent_path();
                    m_NeedRefresh = true;
                }
            }

            // 刷新按钮（可选）
            ImGui::SameLine();
            if (ImGui::Button("Refresh"))
            {
                m_NeedRefresh = true;
            }

            // 显示当前路径
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", CurPath.string().c_str());

            // 刷新缓存
            RefreshDirectoryCache();

            ImGui::Columns(colCount, nullptr, false);

            for (size_t i = 0; i < m_Cache.size(); i++)
            {
                const auto& entry = m_Cache[i];

                // 使用PushID避免文件名冲突
                ImGui::PushID(static_cast<int>(i));

                // 选择图标(展示的均为已注册资源)
                const auto& img = entry.isDirectory ? icon.Folder : icon.File;

                // 渲染图标按钮
                ImGui::ImageButton("##icon", (ImTextureRef)img.id,
                    { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

                // 双击处理
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    if (entry.isDirectory)
                    {
                        CurPath /= entry.fileName;
                        m_NeedRefresh = true;
                    }
                }

                // 右键菜单（可选）
                if (ImGui::BeginPopupContextItem())
                {
                    if (!entry.isDirectory)
                    {
                        if (ImGui::MenuItem("Copy Asset ID"))
                        {
                            ImGui::SetClipboardText(entry.assetId.GetID().c_str());
                        }
                    }
                    ImGui::EndPopup();
                }

                // 拖拽源
                if (!entry.isDirectory)
                {
                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                    {
                        std::string path = entry.path.string();
                        ImGui::SetDragDropPayload("CONTENT_BROWSER_DRAG_ITEM",
                            path.data(), (path.size() + 1) * sizeof(char));

                        // 也可以传递AssetID
                        ImGui::SetDragDropPayload("ASSET_ID",
                            &entry.assetId, sizeof(AssetID));

                        ImGui::Text("%s", entry.fileName.c_str());
                        ImGui::EndDragDropSource();
                    }
                }

                // 文件名显示
                ImGui::TextWrapped("%s", entry.fileName.c_str());

               

                ImGui::PopID();
                ImGui::NextColumn();
            }

            // 空目录提示
            if (m_Cache.empty())
            {
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
                    "No registered assets in this directory");
            }
        }
        ImGui::Columns(1);
        ImGui::End();
    }
}