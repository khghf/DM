#pragma once
#include"AssetLoader.h"
namespace DM
{
	struct  ShaderLoader:AssetLoader
	{
		static SPtr<AssetObject>Load(std::string_view path);
		static SPtr<AssetObject>Load(AssetPack* pack);
	};
}