#include "DMPCH.h"
#include "Config.h"
#include<json/json.hpp>
#include<string_view>
#include<fstream>
//#include"Tool/Util/Util.h"
namespace DM
{
	static std::string ConfigFilePathAbs = "";//配置文件绝对路径
	static std::string ConfigFolderPathAbs = "";//配置文件所属文件夹的绝对路径
	static nlohmann::json js;
	void Config::Init(const std::string& configFilePath)
	{
		std::ifstream ConfigFile;
		ConfigFile.open(configFilePath);
		if (ConfigFile.is_open())
		{
			ConfigFile >> js;
			ConfigFile.close();
		}
		else
		{
			LOG_CORE_WARN("Config file not found: {}, using defaults", configFilePath);
			// 填充默认值，避免后续 js["key"] 访问时抛出 nlohmann::json::type_error
			js = {
				{"Assets", 
					{
						{"Folder", "Assets"},
						{"Shader", "Assets/Shader"},
						{"Texture", "Assets/Texture"},
						{"Font", "Assets/Font"},
						{"Icon", "Assets/Icon"},
						{"Save", "Assets/Save"}
					}
				},
				{"Cache", 
					{
						{"Folder", "Cache"},
						{"Shader", "Cache/Shader"}
					}
				}
			};
		}

		// 解析配置文件所在目录的绝对路径
		try
		{
			std::filesystem::path p = std::filesystem::absolute({ configFilePath });
			if (std::filesystem::exists(p))
			{
				ConfigFilePathAbs = std::filesystem::canonical(p).string();
				ConfigFolderPathAbs = std::filesystem::canonical(p.parent_path()).string();
			}
			else
			{
				// 文件不存在时回退到当前工作目录
				ConfigFilePathAbs = p.string();
				ConfigFolderPathAbs = std::filesystem::current_path().string();
			}
		}
		catch (const std::exception& e)
		{
			LOG_CORE_ERROR("Config path resolution failed: {}", e.what());
			ConfigFolderPathAbs = std::filesystem::current_path().string();
		}
	}

	std::string Config::AssetFolder()
	{
		return ConfigFolderPathAbs +"/" + std::string(js["Assets"]["Folder"]);
	}

	std::string Config::AssetShader()
	{
		return ConfigFolderPathAbs + "/" + std::string(js["Assets"]["Shader"]);
	}

	std::string Config::AssetTexture()
	{
		return ConfigFolderPathAbs + "/" + std::string(js["Assets"]["Texture"]);
	}

	std::string Config::AssetFont()
	{
		return ConfigFolderPathAbs + "/" + std::string(js["Assets"]["Font"]);
	}

	std::string Config::AssetSave()
	{
		return ConfigFolderPathAbs + "/" + std::string(js["Assets"]["Save"]);
	}

	std::string Config::AssetIcon()
	{
		return ConfigFolderPathAbs + "/" + std::string(js["Assets"]["Icon"]);
	}

	std::string Config::CacheFolder()
	{
		return ConfigFolderPathAbs + "/" + std::string(js["Cache"]["Folder"]);
	}

	std::string Config::CacheShaderFolder()
	{
		return ConfigFolderPathAbs +"/" + std::string(js["Cache"]["Shader"]);
	}

}