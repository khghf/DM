#pragma once
#include"AssetImporter.h"
namespace DM
{
	struct DM_API Texture2DImporter:public AssetImporter
	{
		static AssetPack* Import(std::string_view assetPath);
	};
}