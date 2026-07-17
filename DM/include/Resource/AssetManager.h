#pragma once
#include<Tool/ISingletion.h>
#include"AssetRegistry.h"
namespace DM
{
    class Asset;
    class DM_API AssetManager :public ISingletion<AssetManager>
    {
        friend class ISingletion<AssetManager>;
        using UTextureRegistry  = AssetRegistry<Texture2D>;	    UTextureRegistry Texture2DRegistry;
        using UShaderRegistry   = AssetRegistry<Shader>;		UShaderRegistry ShaderRegistry;
    public:
        ~AssetManager();
    public:
        void LoadAsset();
        void PostLoadAsset();
    private:
        AssetManager();
        template<typename T>
        void RegisterAsset(const std::vector<std::string>& Paths, AssetRegistry<T>& Registry);
    public:
        template<typename T>
        SPtr<T>LoadAsset(const std::string& path);//加载的资产不会存储在管理器内部
        template<typename T>
        SPtr<T>RigersterAsset(const std::string& path);//加载的资产会存储在管理器内部
        template<typename T>
        SPtr<T>GetAsset(const std::string& Name)const;
        template<typename T>
        SPtr<T>GetAsset(const char str[])const;
        template<typename T>
        std::vector<SPtr<T>>GetAllAsset()const;
    };
    template<typename T>
    inline void AssetManager::RegisterAsset(const std::vector<std::string>& Paths, AssetRegistry<T>& Registry)
    {
        if (Paths.size() == 0)return;
        for (const auto& path : Paths)
        {
            Registry.Register(path);
        }
    }
    template<typename T>
    inline SPtr<T> AssetManager::LoadAsset(const std::string& path)  { return T::Create(path); }
    template<>
    inline SPtr<Texture2D> AssetManager::RigersterAsset<Texture2D>(const std::string& path) {return Texture2DRegistry.Register(path);}
    template<>
    inline SPtr<Shader> AssetManager::RigersterAsset<Shader>(const std::string& path) { return ShaderRegistry.Register(path); }
   
    template<>
    inline SPtr<Texture2D> AssetManager::GetAsset<Texture2D>(const std::string& Name)const {return Texture2DRegistry.GetAsset(Name);}
    template<>
    inline SPtr<Shader> AssetManager::GetAsset<Shader>(const std::string& Name)const { return ShaderRegistry.GetAsset(Name); }
    template<typename T>
    inline SPtr<T> AssetManager::GetAsset(const char str[]) const { std::string Str(str); return GetAsset<T>(Str); }
    template<>
    inline std::vector<SPtr<Texture2D>> AssetManager::GetAllAsset() const {return Texture2DRegistry.GetAllAsset();}
    template<>
    inline std::vector<SPtr<Shader>> AssetManager::GetAllAsset() const { return ShaderRegistry.GetAllAsset(); }
}


