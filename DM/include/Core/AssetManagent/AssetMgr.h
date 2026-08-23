#pragma once
#include<filesystem>
#include"AssetImporter/AssetImporter.h"
#include"AssetLoader/AssetLoader.h"
#include"AssetObject/Shader.h"
#include"AssetObject/Texture2D.h"
#include"AssetObject/Model.h"
#include<Framework/Base/World.h>
#include"AssetMetaDatabase.h"
#include"AssetCache.h"
#include<Foundation/FileSystem.h>
namespace DM
{
    class DM_API AssetMgr
    {
        using Importer = AssetPack*(*)(std::string_view);
    public:
        ~AssetMgr()=default;
        static AssetMgr* Get()
        {
            static AssetMgr inst ;
            return &inst;
        }
    public:
        static AssetID TryGetAssetID(std::string_view sourceFilePath);

        template<typename T>
        static SPtr<T>      LoadAsset( std::string_view  path);
    protected:
        static bool FileExist(std::string_view path);
        static bool CheckSourceFileModified(AssetPack* pack);
        static bool CheckSourceFileModified(std::string_view sourceFilePath);
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
        //从离线资产数据中查找ID
        AssetID assetId = TryGetAssetID(path);


        AssetMetaDatabase* database = AssetMetaDatabase::Get();
        AssetCache* cache = AssetCache::Get();
        SPtr<T>asset{};
        if (assetId)
        {
            //查询缓存
            if (asset = cache->GetAsset<T>(assetId )) return asset;
            
            if (CheckSourceFileModified(path.data()))
            {
                AssetPack* assetPack = AssetImporter::Import(path);
                std::string packPath = AssetUtil::SerializePack(assetPack);//序列化到磁盘
                asset = std::static_pointer_cast<T>(AssetLoader::Load(assetPack));
            }
            else
            {
                //Id有效直接加载资产包
                std::string assetPath = database->GetAssetPathByGuid(assetId);
                asset = std::static_pointer_cast<T>(AssetLoader::Load(assetPath));
            }

        }
        else
        {
            //Id无效导入源文件生成资产包
            std::filesystem::path p{path};
            //FileSystem::GetExtension/
            AssetPack*assetPack= AssetImporter::Import(path);

            asset = std::static_pointer_cast<T>(AssetLoader::Load(assetPack));

            assetId = assetPack->GetGUID();

            std::string packPath = AssetUtil::SerializePack(assetPack);//序列化到磁盘

            delete assetPack;
        }

        cache->CacheAsset(assetId,asset);

        return asset;
    }
}


