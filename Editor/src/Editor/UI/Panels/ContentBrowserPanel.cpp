#include <DMPCH.h>
#include "Editor/UI/Panels/ContentBrowserPanel.h"
#include <Editor/UI/EditorUI.h>
#include <Core/Config/Paths.h>
#include <Platform/Input/Input.h>
#include <Core/AssetManagent/AssetMgr.h>
#include <Core/AssetManagent/AssetMetaDatabase.h>
#include <Core/AssetManagent/AssetImporter/AssetImporter.h>
#include <Core/Log.h>
#include <Editor/Services/AssetScanner.h>
#include <Editor/Core/EditorContext.h>
#include <Editor/Services/SelectionManager.h>
#include <Editor/UI/Widgets/DragItem.h>
#include <Editor/Services/IClipboard.h>
#include <Editor/Commands/ICommand.h>
#include <Editor/Commands/CommandInvoker.h>
#include <algorithm>
#include <map>

#ifdef ENABLE_VULKAN_API
#include <backends/imgui_impl_vulkan.h>
#include <Core/RHI/Backend/Vulkan/VulkanTexture.h>
#endif
#include<Core/EventBus/Event/Event.h>
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
        if (!icon.Texture) return;
        RHI::VulkanTexture* vTexture = static_cast<RHI::VulkanTexture*>(icon.Texture->GetRHIResource());
        if (!vTexture) return;
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
        // 以规范化后的相对路径为基准(Assets)，与数据库记录/目录归属判断保持一致
        CurPath = AssetMetaDatabase::NormalizePath(Paths::AssetRoot().string());

        // 内置图标：导入属于编辑器侧职责，AssetMgr 只做加载，
        // 因此加载前需确保图标已导入注册(未导入则在此显式导入)
        const std::string fileIconPath = (Paths::AssetRoot() / "Icon" / "File.png").string();
        const std::string folderIconPath = (Paths::AssetRoot() / "Icon" / "Folder.png").string();
        if (!AssetMgr::TryGetAssetID(fileIconPath).IsValid())
            EditorContext::GetService<AssetScanner>()->Import(fileIconPath);
        if (!AssetMgr::TryGetAssetID(folderIconPath).IsValid())
            EditorContext::GetService<AssetScanner>()->Import(folderIconPath);

        icon.File.Texture = AssetMgr::LoadAsset<Texture2D>(fileIconPath);
        icon.Folder.Texture = AssetMgr::LoadAsset<Texture2D>(folderIconPath);

        AddTexture(icon.File);
        AddTexture(icon.Folder);
    }

    ContentBrowserPanel::~ContentBrowserPanel()
    {
        RemoveTexture(icon.File);
        RemoveTexture(icon.Folder);
    }

    void ContentBrowserPanel::HandleEvent(Event* const e)
    {
        if (e->GetResourceType() == EEventType::AssetDatabaseChanged)
        {
            m_NeedRefresh = true;
        }
    }

    std::vector<std::string> ContentBrowserPanel::BuildOrderedPathList() const
    {
        std::vector<std::string> ordered;
        ordered.reserve(m_Cache.size());
        for (const auto& entry : m_Cache)
        {
            ordered.push_back(entry.path.string());
        }
        return ordered;
    }

    void ContentBrowserPanel::RefreshDirectoryCache()
    {
        if (!m_NeedRefresh) return;

        auto database = AssetMetaDatabase::Get();

        m_Cache.clear();

        fs::path curPath = database->NormalizePath(CurPath.string());

        std::map<std::string, CachedEntry> fileEntries;
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
        EditorUI::Begin("Content Browser");
        {
            static float padding = 4.f;
            static float thumbnailSize = 80.f;
            static float cellSize = thumbnailSize + padding;

            float panelWidth = EditorUI::GetContentRegionAvail().x;
            int colCount = (int)(panelWidth / cellSize);
            colCount = colCount > 0 ? colCount : 1;

            // 返回按钮(返回上级目录时清空路径选中)
            if (CurPath != AssetMetaDatabase::NormalizePath(Paths::AssetRoot().string()))
            {
                if (EditorUI::Button("<-"))
                {
                    CurPath = CurPath.parent_path();
                    m_NeedRefresh = true;
                    EditorContext::GetService<SelectionManager>()->ClearPaths();
                }
                else if (EditorUI::IsKeyReleased(ImGuiKey_MouseX1) && EditorUI::IsWindowHovered())
                {
                    CurPath = CurPath.parent_path();
                    m_NeedRefresh = true;
                    EditorContext::GetService<SelectionManager>()->ClearPaths();
                }
            }

            // 刷新按钮
            EditorUI::SameLine();
            if (EditorUI::Button("Refresh"))
            {
                m_NeedRefresh = true;
            }

            // 剪贴板状态栏：粘贴到当前目录 / 取消剪切
            IClipboard* clip = EditorContext::GetService<IClipboard>();
            if (clip && clip->HasContent())
            {
                EditorUI::SameLine();
                const char* mode = clip->IsCut() ? "[Cut]" : "[Copy]";
                EditorUI::TextColored(clip->IsCut() ? ImVec4(0.95f, 0.65f, 0.2f, 1.0f) : ImVec4(0.6f, 0.8f, 1.0f, 1.0f),
                    "%s %zu item(s)", mode, clip->GetPaths().size());
                if (EditorUI::IsItemHovered())
                    EditorUI::SetTooltip("Paste into this directory (Ctrl+V); Esc/Cancel to clear");
                EditorUI::SameLine();
                if (EditorUI::Button("Paste##clip"))
                {
                    CommandInvoker::Invoke<CmdPasteAsset>((Paths::ProjectRoot() / CurPath).string());
                }
                EditorUI::SameLine();
                if (EditorUI::Button("Cancel##clip"))
                {
                    clip->Clear();
                }
            }

            // 显示当前路径
            EditorUI::SameLine();
            EditorUI::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", CurPath.string().c_str());

            // 刷新缓存
            RefreshDirectoryCache();

            EditorUI::Columns(colCount, nullptr, false);

            for (size_t i = 0; i < m_Cache.size(); i++)
            {
                const auto& entry = m_Cache[i];

                // 使用PushID避免文件名冲突
                EditorUI::PushID(i);

                // 选择图标(展示的均为已注册资源)
                const auto& img = entry.isDirectory ? icon.Folder : icon.File;

                // 剪切视觉提示：被剪切的资产以低透明度显示
                const bool isCut = clip->IsCut() && clip->Contains(entry.path.string());
                if (isCut)
                    EditorUI::PushStyleVar(ImGuiStyleVar_Alpha, 0.35f);

                // 渲染图标按钮：FramePadding 置 0，避免按钮实际宽度(缩略图+内边距)超过列宽，
                // 否则当面板宽度接近 colCount*cellSize 时平均列宽被列间距压缩，右侧图标会盖住左侧图标的右缘
                EditorUI::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
                EditorUI::ImageButton("##icon", (ImTextureRef)img.id,
                    { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
                EditorUI::PopStyleVar();
                if (isCut)
                    EditorUI::PopStyleVar();

                // 选中高亮：自绘外边框(向四周扩展 1px，避免 ImageButton 内边框右侧被列边界裁剪)
                const bool isSelected = EditorContext::GetService<SelectionManager>()->ContainsPath(entry.path.string());
                if (isSelected)
                {
                    ImVec2 min = EditorUI::GetItemRectMin();
                    ImVec2 max = EditorUI::GetItemRectMax();
                    min.x -= 1.0f; min.y -= 1.0f;
                    max.x += 1.0f; max.y += 1.0f;
                    EditorUI::GetWindowDrawList()->AddRect(min, max, IM_COL32(255, 200, 0, 255), 0.0f, ImDrawFlags_None, 2.0f);
                }

                // 单击选择：Ctrl 切换选中、Shift 范围选择、默认单选(状态统一存于 SelectionManager)
                if (EditorUI::IsItemClicked(ImGuiMouseButton_Left))
                {
                    const bool ctrl = EditorUI::GetIO().KeyCtrl;
                    const bool shift = EditorUI::GetIO().KeyShift;
                    const std::string path = entry.path.string();
                    SelectionManager* sel = EditorContext::GetService<SelectionManager>();

                    if (ctrl)
                    {
                        sel->TogglePath(path);
                    }
                    else if (shift)
                    {
                        sel->SelectPathRange(BuildOrderedPathList(), path);
                    }
                    else
                    {
                        sel->SetSelectedPath(path);
                    }
                }

                // 双击处理
                if (EditorUI::IsItemHovered() && EditorUI::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    if (entry.isDirectory)
                    {
                        CurPath /= entry.fileName;
                        m_NeedRefresh = true;
                        EditorContext::GetService<SelectionManager>()->ClearPaths();
                    }
                }

                // 右键菜单（可选）
                if (EditorUI::BeginPopupContextItem())
                {
                    SelectionManager* sel = EditorContext::GetService<SelectionManager>();
                    if (EditorUI::MenuItem("Duplicate", "Ctrl+D"))
                    {
                        sel->SetSelectedPath(entry.path.string());
                        CommandInvoker::Invoke<CmdDuplicateAsset>();
                    }
                    if (EditorUI::MenuItem("Cut", "Ctrl+X"))
                    {
                        sel->SetSelectedPath(entry.path.string());
                        CommandInvoker::Invoke<CmdCutAsset>();
                    }
                    if (EditorUI::MenuItem("Copy", "Ctrl+C"))
                    {
                        sel->SetSelectedPath(entry.path.string());
                        CommandInvoker::Invoke<CmdCopyAsset>();
                    }
                    if (clip->HasContent())
                    {
                        if (EditorUI::MenuItem("Paste", "Ctrl+V"))
                        {
                            CommandInvoker::Invoke<CmdPasteAsset>((Paths::ProjectRoot() / CurPath).string());
                        }
                    }
                    if (EditorUI::MenuItem("Delete", "Del"))
                    {
                        sel->SetSelectedPath(entry.path.string());
                        CommandInvoker::Invoke<CmdDeleteAsset>();
                    }
                    if (!entry.isDirectory)
                    {
                        if (EditorUI::MenuItem("Copy Asset ID"))
                        {
                            EditorUI::SetClipboardText(entry.assetId.GetID().c_str());
                        }
                    }
                    EditorUI::EndPopup();
                }

                // 拖拽源
                if (!entry.isDirectory)
                {
                    if (EditorUI::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                    {
                        EAssetType type = EAssetType::Unknown;
                        if (const AssetRecord* record = AssetMetaDatabase::Get()->GetRecordByGuid(entry.assetId))
                        {
                            type = record->AssetType;
                        }

                        DragItem item(entry.assetId, type, entry.path.string());
                        item.SetDragDropPayload();

                        EditorUI::Text("%s", entry.fileName.c_str());
                        EditorUI::EndDragDropSource();
                    }
                }

                // 文件名显示(剪切的资产附加 [Cut] 标记)
                if (isCut)
                    EditorUI::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 0.6f));
                EditorUI::TextWrapped("%s%s", entry.fileName.c_str(), isCut ? "  [Cut]" : "");
                if (isCut)
                    EditorUI::PopStyleColor();

               

                EditorUI::PopID();
                EditorUI::NextColumn();
            }

            // 空目录提示
            if (m_Cache.empty())
            {
                EditorUI::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f),
                    "No registered assets in this directory");
            }

            // 点击面板空白区域清空路径选中(不影响实体选中)
            if (EditorUI::IsWindowHovered() && EditorUI::IsMouseClicked(ImGuiMouseButton_Left) && !EditorUI::IsAnyItemHovered())
            {
                EditorContext::GetService<SelectionManager>()->ClearPaths();
            }

            // 资产编辑快捷键：仅在面板悬停时消费，优先于全局实体快捷键(CmdDuplicateEntity/CmdDeleteEntity)
            if (EditorUI::IsWindowHovered())
            {
                SelectionManager* sel = EditorContext::GetService<SelectionManager>();
                if (sel->HasSelectedPath() && EditorUI::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_X))
                    CommandInvoker::Invoke<CmdCutAsset>();
                if (sel->HasSelectedPath() && EditorUI::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_C))
                    CommandInvoker::Invoke<CmdCopyAsset>();
                if (clip->HasContent() && EditorUI::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_V))
                    CommandInvoker::Invoke<CmdPasteAsset>((Paths::ProjectRoot() / CurPath).string());
                if (sel->HasSelectedPath() && EditorUI::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_D))
                    CommandInvoker::Invoke<CmdDuplicateAsset>();
                if (sel->HasSelectedPath() && EditorUI::IsKeyPressed(ImGuiKey_Delete))
                    CommandInvoker::Invoke<CmdDeleteAsset>();
            }
        }
        EditorUI::Columns(1);
        EditorUI::End();
    }
}