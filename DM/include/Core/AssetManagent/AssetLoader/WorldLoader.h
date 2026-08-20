#pragma once
#include"AssetLoader.h"

namespace DM
{
	struct DM_API WorldLoader:AssetLoader
	{
		static SPtr<AssetObject>Load(std::string_view packPath);
		static SPtr<AssetObject>Load(AssetPack* pack);
	};

}