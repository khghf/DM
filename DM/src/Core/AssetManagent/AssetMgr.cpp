#include <Core/AssetManagent/AssetMgr.h>
namespace DM
{
    namespace
    {
        AssetMgr::MissingAssetHandler& GetMissingAssetHandlerRef()
        {
            static AssetMgr::MissingAssetHandler handler;
            return handler;
        }
    }

    void AssetMgr::SetMissingAssetHandler(MissingAssetHandler handler)
    {
        GetMissingAssetHandlerRef() = std::move(handler);
    }

    const AssetMgr::MissingAssetHandler& AssetMgr::GetMissingAssetHandler()
    {
        return GetMissingAssetHandlerRef();
    }

    AssetID::InnerID AssetID::m_InvalidID = "";

    AssetID AssetMgr::TryGetAssetID(std::string_view sourceFilePath)
    {
        // 身份解析下沉到数据库：路径索引 -> 内容哈希索引(覆盖重命名/移动且内容未变)，命中则自动更新路径索引
        return AssetMetaDatabase::Get()->ResolveAssetIDBySourcePath(sourceFilePath);
    }

    bool AssetMgr::FileExist(std::string_view path)
    {
        std::filesystem::path p{ path };
        return std::filesystem::exists(p) && std::filesystem::is_regular_file(p);
    }
}
