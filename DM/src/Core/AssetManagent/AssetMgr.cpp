#include <Core/AssetManagent/AssetMgr.h>
namespace DM
{
    AssetID::InnerID AssetID::m_InvalidID = "";
    AssetID AssetMgr::TryGetAssetID(std::string_view sourceFilePath)
    {
        AssetMetaDatabase* database = AssetMetaDatabase::Get();
        AssetID assetId;

        //根据源文件路径判断是否已导入过该资产
        assetId = database->GetAssetIDBySourceFilePath(sourceFilePath);
        if (assetId.IsValid())return assetId;

        //根据源文件内容判断是否已导入过该资产

        assetId=database->GetAssetIDBySourceFileContent(sourceFilePath);
        if (assetId.IsValid())
        {
            //更新资产数据库
            database->OnSourceFilePathChanged(assetId, sourceFilePath);
            return assetId;
        }

        return assetId;
    }
    bool AssetMgr::CheckSourceFileModified(AssetPack* pack)
    {
        auto metaInfo = pack->GetMeta();
        std::string sourceFilePath = metaInfo.m_SourceFilePath;

        return CheckSourceFileModified(sourceFilePath);
    }
    bool AssetMgr::CheckSourceFileModified( std::string_view sourceFilePath)
    {
        auto database=AssetMetaDatabase::Get();
        uint64_t lastModifiedTime = AssetUtil::GetFileLastModifyTimeStamp(sourceFilePath);

        if (const AssetRecord* record = database->GetRecordBySourceFilePath(sourceFilePath))
        {
            if (lastModifiedTime == record->LastModifyTime)return false;
            if (AssetUtil::Sha256FileContent(sourceFilePath) == record->SourceFileContentHash)return false;
        }
        return true;
    }
    bool AssetMgr::FileExist(std::string_view path)
    {
        std::filesystem::path p{ path };
        return std::filesystem::exists(p) && std::filesystem::is_regular_file(p);
    }
}
