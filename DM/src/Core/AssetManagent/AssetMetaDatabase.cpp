#include <Core/AssetManagent/AssetMetaDatabase.h>
#include <algorithm>

namespace DM
{
    std::filesystem::path AssetMetaDatabase::NormalizePath(std::string_view path)
    {
        std::filesystem::path p(path);
        std::string str = p.lexically_normal().string();

        return std::filesystem::path(str);
    }

    void AssetMetaDatabase::AddNewAssetPack(const AssetPack* pack)
    {
        std::string assetPath = pack->GetMeta().m_SourceFilePath;
        if (pack->GetExtension() != AssetUtil::GetExtension(pack->GetMeta().m_SourceFilePath))
        {
            assetPath += pack->GetExtension();
        }

        AddNewAssetPack(pack->GetGUID(),
            assetPath,
            pack->GetMeta().m_SourceFilePath,
            pack->GetMeta().m_SourceFileContentHash,
            pack->GetMeta().m_SourceFileLastModifyTime,
            pack->GetResourceType());
    }

    void AssetMetaDatabase::AddNewAssetPack(const KeyEvent& guid, const AssetRecord& record)
    {
        AddNewAssetPack(guid, record.AssetPackPath, record.SourceFilePath,
            record.SourceFileContentHash, record.LastModifyTime, record.AssetType);
    }

    void AssetMetaDatabase::AddNewAssetPack(const KeyEvent& guid,
        const std::string& packPath,
        const std::string& sourceFilePath,
        const std::string& sourceFileContentHash,
        uint64_t lastModifyTime, EAssetType type)
    {
        std::unique_lock lock(m_Mutex);

        auto it = m_GuidToRecord.find(guid);
        if (it != m_GuidToRecord.end()) return;

        // 同一源文件路径已注册则跳过，避免同一资源被重复导入产生多条记录
        auto pathIt = m_SourceFilePathToGuid.find(NormalizePath(sourceFilePath));
        if (pathIt != m_SourceFilePathToGuid.end()) return;

        AssetRecord record{ packPath, sourceFilePath, sourceFileContentHash, lastModifyTime, type };
        m_GuidToRecord[guid] = std::move(record);

        AddIndexes(m_GuidToRecord[guid], guid);
        m_IsDataDirty = true;
        m_ModifyCount.fetch_add(1, std::memory_order_relaxed);
    }

    void AssetMetaDatabase::RemoveRecordByGuid(const AssetID& guid)
    {
        std::unique_lock lock(m_Mutex);

        auto it = m_GuidToRecord.find(guid);
        if (it == m_GuidToRecord.end()) return;

        RemoveIndexes(it->second, guid);
        m_GuidToRecord.erase(it);
        m_IsDataDirty = true;
        m_ModifyCount.fetch_add(1, std::memory_order_relaxed);
    }

    void AssetMetaDatabase::RemoveRecordBySourceFilePath(const std::string& sourceFilePath)
    {
        auto id = GetAssetIDBySourceFilePath(sourceFilePath);
        RemoveRecordByGuid(id);
    }

    const AssetRecord* AssetMetaDatabase::GetRecordByGuid(const KeyEvent& guid) const
    {
        std::shared_lock lock(m_Mutex);
        auto it = m_GuidToRecord.find(guid);
        return it != m_GuidToRecord.end() ? &it->second : nullptr;
    }

    const AssetRecord* AssetMetaDatabase::GetRecordBySourceFilePath(std::string_view path) const
    {
        std::shared_lock lock(m_Mutex);
        auto normalizedPath = NormalizePath(path);
        auto it = m_SourceFilePathToGuid.find(normalizedPath);
        return it != m_SourceFilePathToGuid.end() ? GetRecordByGuid(it->second) : nullptr;
    }

    const AssetRecord* AssetMetaDatabase::GetRecordBySourceFileContent(std::string_view filePath) const
    {
        std::shared_lock lock(m_Mutex);
        std::string shaHash = AssetUtil::Sha256FileContent(filePath);
        auto it = m_ContentHashToGuid.find(shaHash);
        return it != m_ContentHashToGuid.end() ? GetRecordByGuid(it->second) : nullptr;
    }

    AssetID AssetMetaDatabase::GetAssetIDBySourceFilePath(std::string_view path) const
    {
        std::shared_lock lock(m_Mutex);
        auto normalizedPath = NormalizePath(path);
        auto it = m_SourceFilePathToGuid.find(normalizedPath);
        return it != m_SourceFilePathToGuid.end() ? it->second : AssetID{};
    }

    AssetID AssetMetaDatabase::GetAssetIDBySourceFileContent(std::string_view filePath) const
    {
        std::shared_lock lock(m_Mutex);
        std::string shaHash = AssetUtil::Sha256FileContent(filePath);
        auto it = m_ContentHashToGuid.find(shaHash);
        return it != m_ContentHashToGuid.end() ? it->second : AssetID{};
    }

    std::string AssetMetaDatabase::GetAssetPathByGuid(const KeyEvent& guid) const
    {
        std::shared_lock lock(m_Mutex);
        auto it = m_GuidToRecord.find(guid);
        return it != m_GuidToRecord.end() ? it->second.AssetPackPath : "";
    }

    std::string AssetMetaDatabase::GetSourceFilePathByGuid(const KeyEvent& guid) const
    {
        std::shared_lock lock(m_Mutex);
        auto it = m_GuidToRecord.find(guid);
        return it != m_GuidToRecord.end() ? it->second.SourceFilePath : "";
    }

    std::vector<std::pair<AssetID, AssetRecord>> AssetMetaDatabase::GetAllRecords() const
    {
        std::shared_lock lock(m_Mutex);
        std::vector<std::pair<AssetID, AssetRecord>> result;
        result.reserve(m_GuidToRecord.size());
        for (const auto& [guid, record] : m_GuidToRecord)
        {
            result.emplace_back(guid, record);
        }
        return result;
    }

    void AssetMetaDatabase::OnSourceFilePathChanged(const AssetID& guid, std::string_view newPath)
    {
        std::unique_lock lock(m_Mutex);

        auto it = m_GuidToRecord.find(guid);
        if (it == m_GuidToRecord.end()) return;

        std::string oldPath = it->second.SourceFilePath;

        // 删除旧索引
        m_SourceFilePathToGuid.erase(NormalizePath(oldPath));

        // 更新记录
        it->second.SourceFilePath = std::string(newPath);

        // 添加新索引
        m_SourceFilePathToGuid[NormalizePath(newPath)] = guid;

        m_IsDataDirty = true;
        m_ModifyCount.fetch_add(1, std::memory_order_relaxed);
    }

    void AssetMetaDatabase::Save()
    {
        std::shared_lock lock(m_Mutex);
        if (IsDirty())
        {
            mirror::Serialize(AssetRootDir + "/" + m_DatabaseFileName, Get());
            m_IsDataDirty = false;
        }
    }

    void AssetMetaDatabase::Load()
    {
        std::unique_lock lock(m_Mutex);

        std::filesystem::path path = AssetRootDir + "/" + m_DatabaseFileName;
        if (!std::filesystem::exists(path))
        {
            std::filesystem::create_directories(path.parent_path());
            std::ofstream out(path);
            out.close();
            return;
        }

        mirror::Deserialize(path.string(), this);
        RebuildIndexes();
    }

    void AssetMetaDatabase::AddIndexes(const AssetRecord& record, const KeyEvent& guid)
    {
        m_SourceFilePathToGuid[NormalizePath(record.SourceFilePath)] = guid;
        m_ContentHashToGuid[record.SourceFileContentHash] = guid;
    }

    void AssetMetaDatabase::RemoveIndexes(const AssetRecord& record, const KeyEvent& guid)
    {
        auto it1 = m_SourceFilePathToGuid.find(NormalizePath(record.SourceFilePath));
        if (it1 != m_SourceFilePathToGuid.end() && it1->second == guid)
        {
            m_SourceFilePathToGuid.erase(it1);
        }

        auto it2 = m_ContentHashToGuid.find(record.SourceFileContentHash);
        if (it2 != m_ContentHashToGuid.end() && it2->second == guid)
        {
            m_ContentHashToGuid.erase(it2);
        }
    }

    void AssetMetaDatabase::RebuildIndexes()
    {
        m_SourceFilePathToGuid.clear();
        m_ContentHashToGuid.clear();

        for (const auto& [guid, record] : m_GuidToRecord)
        {
            m_SourceFilePathToGuid[NormalizePath(record.SourceFilePath)] = guid;
            m_ContentHashToGuid[record.SourceFileContentHash] = guid;
        }
    }
}