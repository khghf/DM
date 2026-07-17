#include<DMPCH.h>
#include "Resource/AssetManager.h"
#include<Tool/Util/Util.h>
#include<type_traits>
#include<Core/Serialization.h>
#include<fstream>
#include "Config.h"
namespace DM
{

    AssetManager::AssetManager()
    {
        LoadAsset();
    }
    AssetManager::~AssetManager()
    {

    }
    

    void AssetManager::LoadAsset()
    {
		const std::string ShaderFloderPath = Config::AssetShader();
		const std::string TextureFloderPath = Config::AssetTexture();
		const std::string IconFloderPath = Config::AssetIcon();
		const std::vector<std::string>& ShaderPaths = Util::GetAllFilePath(ShaderFloderPath, true);
		const std::vector<std::string>& TexturePaths = Util::GetAllFilePath(TextureFloderPath, true);
		const std::vector<std::string>& IconPaths = Util::GetAllFilePath(IconFloderPath, true);
		RegisterAsset(ShaderPaths, ShaderRegistry);
		RegisterAsset(TexturePaths, Texture2DRegistry);
		RegisterAsset(IconPaths, Texture2DRegistry);
        PostLoadAsset();
    }

    void AssetManager::PostLoadAsset()
    {
        //将加载好了的着色器链接成着色程序
        //Array<Ref<Shader>>Shaders = ShaderRegistry.GetAllAsset();
        //std::unordered_map<std::string, Array<Ref<Shader>>>LinkShader;
        //auto GetNameWithOutSuffix = [](const std::string& str)->std::string {
        //    std::string ret;
        //    const auto& Index = str.find_last_of('.');
        //    ret = str.substr(0, Index);
        //    return ret;
        //    };
        //for (const auto& shader : Shaders)
        //{
        //    if (!shader)continue;
        //    std::string name = GetNameWithOutSuffix(shader.get()->GetName());
        //    //Util::Print("0000*********" ,name);
        //    LinkShader[name].push_back(shader);
        //}
        //Array<Ref<Shader>>Programs;
        //for (const auto& In : LinkShader)
        //{
        //    unsigned int  VsId;
        //    unsigned int  FsId;
        //    Shader* s1 = In.second[0].get();
        //    Shader* s2 = In.second[1].get();
        //    if (s1->GetShaderType() == EShaderType::VS)
        //    {
        //        VsId = s1->GetID();
        //        FsId = s2->GetID();
        //    }
        //    else
        //    {
        //        VsId = s2->GetID();
        //        FsId = s1->GetID();
        //    }
        //    Programs.push_back(std::shared_ptr<Shader>(new Shader(VsId, FsId, GetNameWithOutSuffix(s1->GetName()))));
        //}
        //ShaderRegistry.UnRegisterAllAsset();
        //ShaderRegistry.Register(Programs);
    }
}



