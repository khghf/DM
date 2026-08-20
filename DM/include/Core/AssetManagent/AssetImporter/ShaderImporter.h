#pragma once
#include"AssetImporter.h"
namespace DM
{
	struct DM_API ShaderImporter:AssetImporter
	{
		static AssetPack* Import(std::string_view filePath);
	};
}