#include <Core/AssetManagent/AssetMetaDatabase.h>
#include <Core/Config/Paths.h>
#include <Core/EventBus/EventBus.h>
#include <Core/EventBus/Event/AssetDatabaseChangedEvent.h>
#include <algorithm>

namespace DM
{
    namespace
    {
        // 相对路径 -> 绝对路径(以项目根为基准)，供加载侧直接打开文件，消除对工作目录的依赖。
        std::string ResolveToAbsolute(std::string_view path)
        {
            std::filesystem::path p(path);
            return (p.is_absolute() ? p : (Paths::ProjectRoot() / p)).string();
        }
    }

    std::filesystem::path AssetMetaDatabase::NormalizePath(std::string_view path)
    {
        std::filesystem::path p(path);

        // 绝对路径若位于项目根(Paths::ProjectRoot)下，统一转为相对项目根的路径形式。
        // 例：D:\DM\Assets\Save\1.world -> Assets/Save/1.world
        // 数据库记录、扫描查询、内容浏览器目录归属均以此基准，可移植且不依赖工作目录。
        if (p.is_absolute())
        {
            std::filesystem::path rel = std::filesystem::absolute(p).lexically_relative(
                std::filesystem::absolute(Paths::ProjectRoot()));
            if (!rel.empty() && *rel.begin() != "..")
            {
                p = std::move(rel);
            }
        }

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

    void AssetMetaDatabase::AddNewAssetPack(const ID& guid, const AssetRecord& record)
    {
        AddNewAssetPack(guid, record.AssetPackPath, record.SourceFilePath,record.SourceFileContentHash, record.LastModifyTime, record.AssetType);
    }

    void AssetMetaDatabase::AddNewAssetPack(const ID& guid,
        const std::string& packPath,
        const std::string& sourceFilePath,
        const std::string& sourceFileContentHash,
        uint64_t lastModifyTime, EAssetType type)
    {
        std::unique_lock lock(m_Mutex);

        auto it = m_GuidToRecord.find(guid);
        if (it != m_GuidToRecord.end()) return;

        // 写入前统一路径形式(绝对->相对，如 D:\DM\Assets\Save\1.world -> Assets/Save/1.world)，
        const std::string normPackPath = NormalizePath(packPath).string();
        const std::string normSourceFilePath = NormalizePath(sourceFilePath).string();

        // 同一源文件路径已注册则跳过，避免同一资源被重复导入产生多条记录
        auto pathIt = m_SourceFilePathToGuid.find(normSourceFilePath);
        if (pathIt != m_SourceFilePathToGuid.end()) return;

        AssetRecord record{ normPackPath, normSourceFilePath, sourceFileContentHash, lastModifyTime, type };
        m_GuidToRecord[guid] = std::move(record);

        AddIndexes(m_GuidToRecord[guid], guid);
        m_IsDataDirty = true;
        lock.unlock();

        AssetDatabaseChangedEvent evt;
        evt.Data.ChangeType = AssetDatabaseChangedEvent::EChangeType::Add;
        evt.Data.AssetId = guid;
        evt.Data.SourceFilePath = normSourceFilePath;
        EventBus::SendEvent(evt);
    }

    void AssetMetaDatabase::RemoveRecordByGuid(const AssetID& guid)
    {
        std::unique_lock lock(m_Mutex);

        auto it = m_GuidToRecord.find(guid);
        if (it == m_GuidToRecord.end()) return;

        std::string removedSourcePath = it->second.SourceFilePath;
        RemoveIndexes(it->second, guid);
        m_GuidToRecord.erase(it);
        m_IsDataDirty = true;
        lock.unlock();

        AssetDatabaseChangedEvent evt;
        evt.Data.ChangeType = AssetDatabaseChangedEvent::EChangeType::Remove;
        evt.Data.AssetId = guid;
        evt.Data.SourceFilePath = removedSourcePath;
        EventBus::SendEvent(evt);
    }

    void AssetMetaDatabase::RemoveRecordBySourceFilePath(const std::string& sourceFilePath)
    {
        auto id = GetAssetIDBySourceFilePath(sourceFilePath);
        RemoveRecordByGuid(id);
    }

    const AssetRecord* AssetMetaDatabase::GetRecordByGuid(const ID& guid) const
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

    const AssetRecord* AssetMetaDatabase::GetRecordByAssetPackPath(std::string_view packPath) const
    {
        std::shared_lock lock(m_Mutex);
        // 资产包路径在库中唯一，线性查找即可。
        // 归一化比较：兼容历史数据中绝对/相对路径形式混杂的记录，避免绝对/相对失配。
        const std::string normalizedPack = NormalizePath(packPath).string();
        for (const auto& [guid, record] : m_GuidToRecord)
        {
            if (NormalizePath(record.AssetPackPath).string() == normalizedPack) return &record;
        }
        return nullptr;
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

    AssetID AssetMetaDatabase::ResolveAssetIDBySourcePath(std::string_view sourceFilePath)
    {
        // 1. 优先按源文件路径精确匹配
        AssetID assetId = GetAssetIDBySourceFilePath(sourceFilePath);
        if (assetId.IsValid()) return assetId;

        // 2. 按内容哈希匹配(覆盖重命名/移动且内容未变的场景)，命中则更新路径索引
        assetId = GetAssetIDBySourceFileContent(sourceFilePath);
        if (assetId.IsValid())
        {
            OnSourceFilePathChanged(assetId, sourceFilePath);
        }
        return assetId;
    }

    std::string AssetMetaDatabase::GetAssetPathByGuid(const ID& guid) const
    {
        std::shared_lock lock(m_Mutex);
        auto it = m_GuidToRecord.find(guid);
        // 库内统一存相对路径，对外返回绝对路径，加载侧可直接打开文件
        return it != m_GuidToRecord.end() ? ResolveToAbsolute(it->second.AssetPackPath) : "";
    }

    std::string AssetMetaDatabase::GetSourceFilePathByGuid(const ID& guid) const
    {
        std::shared_lock lock(m_Mutex);
        auto it = m_GuidToRecord.find(guid);
        // 库内统一存相对路径，对外返回绝对路径
        return it != m_GuidToRecord.end() ? ResolveToAbsolute(it->second.SourceFilePath) : "";
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

        // 统一路径形式后写入(绝对->相对)
        const std::string newNormPath = NormalizePath(newPath).string();

        // 删除旧索引
        m_SourceFilePathToGuid.erase(NormalizePath(it->second.SourceFilePath));

        // 更新记录
        it->second.SourceFilePath = newNormPath;

        // 添加新索引
        m_SourceFilePathToGuid[newNormPath] = guid;

        m_IsDataDirty = true;
        // 解锁前拷贝事件载荷，避免锁外解引用悬垂迭代器(他线程可能已删除该记录)
        std::string evtSourcePath = it->second.SourceFilePath;
        lock.unlock();

        // 广播数据库变更事件(锁外广播)
        AssetDatabaseChangedEvent evt;
        evt.Data.ChangeType = AssetDatabaseChangedEvent::EChangeType::PathChanged;
        evt.Data.AssetId = guid;
        evt.Data.SourceFilePath = evtSourcePath;
        EventBus::SendEvent(evt);
    }

    void AssetMetaDatabase::OnAssetMoved(const AssetID& guid, std::string_view newSourceFilePath, std::string_view newAssetPackPath)
    {
        std::unique_lock lock(m_Mutex);

        auto it = m_GuidToRecord.find(guid);
        if (it == m_GuidToRecord.end()) return;

        // 统一路径形式后写入(绝对->相对)
        const std::string newSource = NormalizePath(newSourceFilePath).string();
        const std::string newPack = NormalizePath(newAssetPackPath).string();

        // 删除旧路径索引，更新记录，重建新索引
        m_SourceFilePathToGuid.erase(NormalizePath(it->second.SourceFilePath));
        it->second.SourceFilePath = newSource;
        it->second.AssetPackPath = newPack;
        m_SourceFilePathToGuid[newSource] = guid;

        m_IsDataDirty = true;
        // 解锁前拷贝事件载荷，避免锁外解引用悬垂迭代器(他线程可能已删除该记录)
        std::string evtSourcePath = it->second.SourceFilePath;
        lock.unlock();

        // 广播数据库变更事件(锁外广播)
        AssetDatabaseChangedEvent evt;
        evt.Data.ChangeType = AssetDatabaseChangedEvent::EChangeType::PathChanged;
        evt.Data.AssetId = guid;
        evt.Data.SourceFilePath = evtSourcePath;
        EventBus::SendEvent(evt);
    }

    void AssetMetaDatabase::Save()
    {
        std::shared_lock lock(m_Mutex);
        if (IsDirty())
        {
            mirror::Serialize((Paths::AssetRoot() / m_DatabaseFileName).string(), Get());
            m_IsDataDirty = false;
        }
    }

    void AssetMetaDatabase::Load()
    {
        std::unique_lock lock(m_Mutex);

        std::filesystem::path path = Paths::AssetRoot() / m_DatabaseFileName;
        if (!std::filesystem::exists(path))
        {
            std::filesystem::create_directories(path.parent_path());
            std::ofstream out(path);
            out.close();
            return;
        }

        mirror::Deserialize(path.string(), this);

        // 迁移历史数据中的绝对路径(如 D:\DM\Assets\Save\1.world)为相对形式，
        // 保证磁盘记录可移植，并与内容浏览器/资产管线的相对路径基准一致。
        // 落于资产根目录之外的绝对路径保持不变(资源本就不属于资产库浏览范围)。
        for (auto& [guid, record] : m_GuidToRecord)
        {
            const std::string normSource = NormalizePath(record.SourceFilePath).string();
            if (normSource == record.SourceFilePath) continue;

            if (record.AssetPackPath.rfind(record.SourceFilePath, 0) == 0)
            {
                // AssetPackPath 与 SourceFilePath 仅差后缀(.dasset)时，保留差异
                record.AssetPackPath = normSource + record.AssetPackPath.substr(record.SourceFilePath.length());
            }
            else
            {
                record.AssetPackPath = NormalizePath(record.AssetPackPath).string();
            }
            record.SourceFilePath = normSource;
            m_IsDataDirty = true;
        }

        RebuildIndexes();
    }

    void AssetMetaDatabase::AddIndexes(const AssetRecord& record, const ID& guid)
    {
        m_SourceFilePathToGuid[NormalizePath(record.SourceFilePath)] = guid;
        m_ContentHashToGuid[record.SourceFileContentHash] = guid;
    }

    void AssetMetaDatabase::RemoveIndexes(const AssetRecord& record, const ID& guid)
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