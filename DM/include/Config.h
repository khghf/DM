#pragma once
#include <string>
#include<filesystem>
#include<Core/Core.h>
namespace DM
{
	class DM_API Config
	{
	public:
		static void Init(const std::string& configFilePath = "../Config.json");
		static std::string				AssetFolder();
		static std::string				AssetShader();
		static std::string				AssetTexture();
		static std::string				AssetFont();
		static std::string				AssetSave();
		static std::string				AssetIcon();




		static std::string				CacheFolder();
		static std::string				CacheShaderFolder();
	};
}


