#pragma once
#include"../AssetObject/AssetObject.h"

namespace DM
{
	/// <summary>
	/// 从.dasset中加载资源
	/// </summary>
	struct DM_API AssetLoader
	{

		static SPtr<AssetObject>Load(std::string_view packPath);
		static SPtr<AssetObject>Load(AssetPack* pack);

		using Loader_Path = SPtr<AssetObject>(*)(std::string_view);
		using Loader_Pack = SPtr<AssetObject>(*)(AssetPack*);

		static std::unordered_map<EAssetType, Loader_Path>& GetLoaderRegistry_Path()
		{
			static std::unordered_map<EAssetType, Loader_Path> registry;
			return registry;
		}
		static std::unordered_map<EAssetType, Loader_Pack>& GetLoaderRegistry_Pack()
		{
			static std::unordered_map<EAssetType, Loader_Pack> registry;
			return registry;
		}
	private:



		static Loader_Path SelectLoader(std::string_view packPath);
		static Loader_Pack SelectLoader(AssetPack*pack);
	};

	struct LoaderRegister_Path
	{
		LoaderRegister_Path(EAssetType type, AssetLoader::Loader_Path loader)
		{
			auto& registry = AssetLoader::GetLoaderRegistry_Path();
			registry[type] = loader;
		}
	};

	struct LoaderRegister_Pack
	{
		LoaderRegister_Pack(EAssetType type, AssetLoader::Loader_Pack loader)
		{
			auto& registry = AssetLoader::GetLoaderRegistry_Pack();
			registry[type] = loader;
		}
	};

}




#define _REGISTER_LOAD_PATH(AssetType,Loader,ID)inline static LoaderRegister_Path _CONCAT_(register_,ID){AssetType,Loader};
#define _REGISTER_LOAD_PACK(AssetType,Loader,ID)inline static LoaderRegister_Pack _CONCAT_(register_,ID){AssetType,Loader};

#define REGISTER_LOAD_PATH(AssetType,Loader)	_REGISTER_LOAD_PATH(AssetType,Loader,__COUNTER__)

#define REGISTER_LOAD_PACK(AssetType,Loader)	_REGISTER_LOAD_PACK(AssetType,Loader,__COUNTER__)

