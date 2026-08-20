#pragma once
#include"AssetImporter.h"
namespace DM
{
	struct DM_API ModelImporter:AssetImporter
	{
		static AssetPack* Import(std::string_view sourceFilePath);
	};
}