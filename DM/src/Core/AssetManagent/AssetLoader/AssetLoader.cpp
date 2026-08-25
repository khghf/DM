#include<Core/AssetManagent/AssetLoader/Texture2DLoader.h>
#include<Core/AssetManagent/AssetLoader/ModelLoader.h>
#include<Core/AssetManagent/AssetLoader/ShaderLoader.h>
#include<Core/AssetManagent/AssetMetaDatabase.h>
namespace DM
{
	

	AssetLoader::Loader_Path AssetLoader::SelectLoader(std::string_view packPath)
	{
		// 1. 优先按资产包路径精确匹配
		//    World 等自包含资产的资产包路径即源文件路径没有 .dasset 后缀
		const AssetRecord* record = AssetMetaDatabase::Get()->GetRecordByAssetPackPath(packPath);

		// 2.剥掉 ".dasset" 后缀按源文件路径匹配
		if (!record)
		{
			std::string sourceFilePath(packPath);   // 注意：勿用 packPath.data()，string_view 不保证以 \0 结尾
			constexpr std::string_view dasSuffix = ".dasset";
			if (sourceFilePath.size() > dasSuffix.size()&& sourceFilePath.compare(sourceFilePath.size() - dasSuffix.size(), dasSuffix.size(), dasSuffix) == 0)
			{
				sourceFilePath.resize(sourceFilePath.size() - dasSuffix.size());
			}
			record = AssetMetaDatabase::Get()->GetRecordBySourceFilePath(sourceFilePath);
		}

		if (!record)
		{
			LOG_CORE_ERROR("[AssetLoader]no asset record found for pack path: {}", packPath);
			return nullptr;
		}

		const auto& registry = GetLoaderRegistry_Path();
		auto it = registry.find(record->AssetType);
		return it != registry.end() ? it->second : nullptr;
	}

	AssetLoader::Loader_Pack AssetLoader::SelectLoader(AssetPack* pack)
	{
		if (!pack)return nullptr;

		const auto& registry = GetLoaderRegistry_Pack();
		auto it = registry.find(pack->GetResourceType());
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