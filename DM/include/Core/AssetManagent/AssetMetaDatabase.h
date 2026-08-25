// AssetMetaDatabase.h
#pragma once
#include "AssetObject/AssetObject.h"
#include <unordered_map>
#include <shared_mutex>
#include <filesystem>
#include <vector>
#include <utility>
#include <atomic>

namespace DM
{
    /// <summary>
    /// 资产记录
    /// </summary>
    struct AssetRecord
    {
        std::string AssetPackPath;          // 资产包路径(引擎内部 .asset 文件路径)
        std::string SourceFilePath;         // 源文件路径(原始 FBX/PNG 等路径)
        std::string SourceFileContentHash;  // 源文件内容哈希(SHA256，用于去重/改名检测)
        uint64_t    LastModifyTime;         // 源文件最后修改时间
        EAssetType  AssetType;              // 资产类型(纹理/网格/材质等)
    };
    REGISTER_TYPE(AssetRecord);
    REGISTER_MEMBER(AssetRecord, AssetPackPath);
    REGISTER_MEMBER(AssetRecord, SourceFilePath);
    REGISTER_MEMBER(AssetRecord, SourceFileContentHash);
    REGISTER_MEMBER(AssetRecord, LastModifyTime);
    REGISTER_MEMBER(AssetRecord, AssetType);

    /// <summary>
    /// 资产元数据库
    /// 
    /// 职责：
    ///   1. 记录资产包路径 ↔ 源文件路径 ↔ GUID 的映射关系
    ///   2. 通过内容哈希检测重复/重命名的源文件
    ///   3. 持久化到磁盘(AssetDatabase.txt)
    /// 
    /// 索引结构：
    ///   m_GuidToRecord          ：GUID -> 完整记录(主表)
    ///   m_SourceFilePathToGuid  ：源文件路径 -> GUID(路径索引)
    ///   m_ContentHashToGuid     ：内容哈希 -> GUID(内容索引，用于去重)
    /// </summary>
    class DM_API AssetMetaDatabase
    {
        using ID = AssetID;  // GUID 类型别名

    private:
        AssetMetaDatabase()
        {
            Load();  // 构造时从磁盘加载
        }

    public:
        ~AssetMetaDatabase()
        {
            Save();  // 析构时保存
        }

        static AssetMetaDatabase* Get()
        {
            static AssetMetaDatabase inst;
            return &inst;
        }

        /// <summary>
        /// 从资产包添加记录(自动推断路径和类型)
        /// </summary>
        void AddNewAssetPack(const AssetPack* pack);

        /// <summary>
        /// 从记录添加
        /// </summary>
        void AddNewAssetPack(const ID& guid, const AssetRecord& record);

        /// <summary>
        /// 手动指定所有字段添加
        /// </summary>
        /// <param name="guid">资产 GUID</param>
        /// <param name="packPath">资产包路径</param>
        /// <param name="sourceFilePath">源文件路径</param>
        /// <param name="sourceFileContentHash">源文件内容哈希</param>
        /// <param name="lastModifyTime">最后修改时间</param>
        /// <param name="type">资产类型</param>
        void AddNewAssetPack(const ID& guid, const std::string& packPath,
            const std::string& sourceFilePath,
            const std::string& sourceFileContentHash,
            uint64_t lastModifyTime, EAssetType type);

        /// <summary>
        /// 删除指定 GUID 的记录(同时清理索引)
        /// </summary>
        void RemoveRecordByGuid(const AssetID& guid);

        /// <summary>
        /// 根据源文件路径删除记录。
        /// </summary>
        /// <param name="sourceFilePath">要删除记录的源文件路径。</param>
        void RemoveRecordBySourceFilePath(const std::string& sourceFilePath);

        /// <summary>
        /// 通过 GUID 查询记录
        /// </summary>
        const AssetRecord* GetRecordByGuid(const ID& guid) const;

        /// <summary>
        /// 通过源文件路径查询记录
        /// </summary>
        const AssetRecord* GetRecordBySourceFilePath(std::string_view path) const;

        /// <summary>
        /// 通过资产包路径精确查询记录。
        /// 注：World 等自包含资产的资产包路径即源文件路径(无 .dasset 后缀)，
        /// 与 Shader/Texture/Model 的 "xxx.ext.dasset" 格式不同，必须按包路径精确匹配。
        /// </summary>
        const AssetRecord* GetRecordByAssetPackPath(std::string_view packPath) const;

        /// <summary>
        /// 通过源文件内容查询记录(计算SHA256后匹配)
        /// </summary>
        const AssetRecord* GetRecordBySourceFileContent(std::string_view filePath) const;

        /// <summary>
        /// 通过源文件路径获取GUID
        /// </summary>
        AssetID GetAssetIDBySourceFilePath(std::string_view path) const;

        /// <summary>
        /// 通过源文件内容获取GUID
        /// </summary>
        AssetID GetAssetIDBySourceFileContent(std::string_view filePath) const;

        /// <summary>
        /// 解析源文件路径对应的资产ID：
        /// 1. 优先按源文件路径精确匹配
        /// 2. 未命中时按内容哈希匹配(覆盖重命名/移动且内容未变的场景)，命中则自动更新路径索引
        /// 3. 均未命中返回无效ID
        /// 供加载侧(AssetMgr)与导入侧(AssetImporter)共用，保证身份解析逻辑唯一。
        /// </summary>
        AssetID ResolveAssetIDBySourcePath(std::string_view sourceFilePath);

        /// <summary>
        /// 通过 GUID 获取资产包路径
        /// </summary>
        std::string GetAssetPathByGuid(const ID& guid) const;

        /// <summary>
        /// 通过 GUID 获取源文件路径
        /// </summary>
        std::string GetSourceFilePathByGuid(const ID& guid) const;

        /// <summary>
        /// 源文件路径变化时调用(文件重命名/移动)
        /// 更新路径索引和记录
        /// </summary>
        void OnSourceFilePathChanged(const AssetID& guid, std::string_view newPath);

        /// <summary>
        /// 资产整体移动时调用(源文件与 .dasset 资产包已物理移动)。
        /// 同步更新 SourceFilePath 与 AssetPackPath 两条路径记录及索引。
        /// </summary>
        void OnAssetMoved(const AssetID& guid, std::string_view newSourceFilePath, std::string_view newAssetPackPath);

        /// <summary>
        /// 保存到磁盘(仅当数据有变化时)
        /// </summary>
        void Save();

        /// <summary>
        /// 从磁盘加载
        /// </summary>
        void Load();

        /// <summary>
        /// 是否有未保存的修改
        /// </summary>
        bool IsDirty() const { return m_IsDataDirty; }

        /// <summary>
        /// 路径规范化(统一分隔符 + 消除 . 和 ..)
        /// </summary>
        static std::filesystem::path NormalizePath(std::string_view path);

        /// <summary>
        /// 获取所有已注册资产记录的快照(GUID + 记录)
        /// 用于以数据库为唯一数据源遍历已注册资源
        /// </summary>
        std::vector<std::pair<AssetID, AssetRecord>> GetAllRecords() const;

    private:
        /// <summary>
        /// 添加反向索引(路径->GUID、内容哈希->GUID)
        /// </summary>
        void AddIndexes(const AssetRecord& record, const ID& guid);

        /// <summary>
        /// 删除反向索引
        /// </summary>
        void RemoveIndexes(const AssetRecord& record, const ID& guid);

        /// <summary>
        /// 从主表重建所有索引(反序列化后调用)
        /// </summary>
        void RebuildIndexes();

        std::unordered_map<ID, AssetRecord> m_GuidToRecord;          // 主表：GUID -> 记录
        std::unordered_map<std::filesystem::path, ID> m_SourceFilePathToGuid;  // 索引：路径 -> GUID
        std::unordered_map<std::string, ID> m_ContentHashToGuid;     // 索引：内容哈希 -> GUID

        mutable std::shared_mutex m_Mutex;

        bool m_IsDataDirty = false;

        std::string m_DatabaseFileName = "AssetDatabase.txt";
    };

    REGISTER_TYPE(AssetMetaDatabase);
    REGISTER_PRIVATE_MEMBER(AssetMetaDatabase, m_GuidToRecord);
}