#pragma once
#include"AssetLoader.h"
namespace DM
{
	struct Texture2DLoader: public AssetLoader
	{
		static SPtr<AssetObject>Load(std::string_view path);
		static SPtr<AssetObject>Load(AssetPack* pack);
	};
}