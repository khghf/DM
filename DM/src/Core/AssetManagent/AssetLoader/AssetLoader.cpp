#include<Core/AssetManagent/AssetLoader/Texture2DLoader.h>
#include<Core/AssetManagent/AssetLoader/ModelLoader.h>
#include<Core/AssetManagent/AssetLoader/ShaderLoader.h>
#include<Core/AssetManagent/AssetMetaDatabase.h>
namespace DM
{
	bool AssetLoader::CheckSourceFileModified(AssetPack* pack)
	{
		auto metaInfo = pack->GetMeta();
		std::string sourceFilePath = metaInfo.m_SourceFilePath;

		uint64_t lastModifiedTime = AssetUtil::GetFileLastModifyTimeStamp(sourceFilePath);

		if (lastModifiedTime == metaInfo.m_SourceFileLastModifyTime)return false;

		if (AssetUtil::Sha256FileContent(sourceFilePath) == metaInfo.m_SourceFileContentHash)return false;

		return true;
	}

	AssetLoader::Loader_Path AssetLoader::SelectLoader(std::string_view packPath)
	{
		std::string sourceFilePath = packPath.data();
			

		auto pos= sourceFilePath.find_last_of('.');
		if (pos != std::string::npos)
		{
			sourceFilePath = sourceFilePath.substr(0, pos);
		}

		const AssetRecord* record = AssetMetaDatabase::Get()->GetRecordBySourceFilePath(sourceFilePath);

		const auto& registry = GetLoaderRegistry_Path();
		auto it = registry.find(record->AssetType);
		return it!= registry.end()?it->second:nullptr;
	}

	AssetLoader::Loader_Pack AssetLoader::SelectLoader(AssetPack* pack)
	{
		if (!pack)return nullptr;

		const auto& registry = GetLoaderRegistry_Pack();
		auto it = registry.find(pack->GetAssetType());
		return it != registry.end() ? it->second : nullptr;
	}

	SPtr<AssetObject>AssetLoader::Load(std::string_view packPath)
	{
		auto loader = SelectLoader(packPath);
		if (!loader)return nullptr;
		return loader(packPath);
	}
	SPtr<AssetObject>AssetLoader::Load(AssetPack* pack)
	{
		auto loader = SelectLoader(pack);
		if (!loader)return nullptr;
		return loader(pack);
	}
}