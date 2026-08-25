#pragma once
#include<filesystem>
#include<functional>
#include"AssetLoader/AssetLoader.h"
#include"AssetObject/Shader.h"
#include"AssetObject/Texture2D.h"
#include"AssetObject/Model.h"
#include<Framework/Base/World.h>
#include"AssetMetaDatabase.h"
#include"AssetCache.h"
#include"AsetPack/AssetPack.h"
#include<Foundation/FileSystem.h>
#include<Core/Log.h>
namespace DM
{
    /// <summary>
    /// 资产管理器：只负责资产的"加载/卸载/缓存"，不承担导入职责。
    /// 导入属于编辑器侧流程(AssetScanner/AssetImporter)，运行时只从已导入的资产包加载。
    /// </summary>
    class DM_API AssetMgr
    {
    public:
        ~AssetMgr()=default;
        static AssetMgr* Get()
        {
            static AssetMgr inst ;
            return &inst;
        }
    public:
        /// <summary>
        /// 解析源文件路径对应的资产ID(路径索引/内容哈希索引)
        /// </summary>
        static AssetID TryGetAssetID(std::string_view sourceFilePath);

        /// <summary>
        /// 缺包处理回调：加载时发现源文件资产包尚未生成(未导入)时，把请求转发给外部注册方。
        /// AssetMgr 不实现导入逻辑，仅发起请求，保持"只负责加载/卸载"的职责边界。
        /// 返回 true 表示已成功导入，调用方将重新解析资产ID。
        /// </summary>
        using MissingAssetHandler = std::function<bool(const std::string& sourceFilePath)>;

        /// <summary>
        /// 注册缺包处理回调(编辑器启动时注册为 AssetScanner::Import)
        /// </summary>
        static void SetMissingAssetHandler(MissingAssetHandler handler);

        /// <summary>
        /// 获取当前缺包处理回调(未注册时返回空，缺包将直接返回空指针并警告)
        /// </summary>
        static const MissingAssetHandler& GetMissingAssetHandler();

        /// <summary>
        /// 加载资产：只做加载，不做导入。
        /// 资产需先经编辑器侧导入并注册到 AssetMetaDatabase 才能加载；
        /// 未导入的源文件路径在存在缺包回调时将按需导入后重试，否则返回空指针并打印警告。
        /// </summary>
        template<typename T>
        static SPtr<T>      LoadAsset( std::string_view  path);
    protected:
        static bool FileExist(std::string_view path);
    private:
        AssetMgr()=default;

    };

    template<typename T>
    inline SPtr<T> AssetMgr::LoadAsset(std::string_view path)
    {
        if (!FileExist(path))
        {
            LOG_CORE_WARN("{}", "Load asset failed path is invalid");
            return {};
        }

        // 解析源文件路径对应的资产ID(路径索引/内容哈希索引，重命名/移动且内容未变可命中)
        AssetID assetId = TryGetAssetID(path);
        if (!assetId.IsValid())
        {
            // 缺包时请求编辑器侧按需导入(AssetMgr 不实现导入，只发起请求)
            const MissingAssetHandler& handler = GetMissingAssetHandler();
            if (handler && handler(std::string(path)))
            {
                assetId = TryGetAssetID(path);
            }

            if (!assetId.IsValid())
            {
                LOG_CORE_WARN("Asset not imported yet, please import in editor first: {}", path);
                return {};
            }
        }

        AssetMetaDatabase* database = AssetMetaDatabase::Get();
        AssetCache* cache = AssetCache::Get();

        // 查询缓存
        if (SPtr<T> asset = cache->GetAsset<T>(assetId)) return asset;

        // 从已导入的资产包(.dasset)加载
        std::string assetPath = database->GetAssetPathByGuid(assetId);
        if (assetPath.empty())
        {
            LOG_CORE_WARN("Asset pack path not found, guid: {}", assetId.GetID());
            return {};
        }

        SPtr<T> asset = std::static_pointer_cast<T>(AssetLoader::Load(assetPath));
        cache->CacheAsset(assetId, asset);

        return asset;
    }
}
