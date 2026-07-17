#pragma once
#include<DMPCH.h>
#include<Core/Core.h>
#include<Tool/Util/Util.h>
#include<Core/Render/Shader.h>
#include<Core/Render/Texture/Texture.h>
namespace DM
{
	template<typename T>
	class AssetRegistry
	{
		std::unordered_map<std::string, SPtr<T>>Registry;
	public:
		inline bool HasAsset(const std::string& name)const;
		inline SPtr<T>GetAsset(const std::string& name)const;
		std::vector<SPtr<T>>GetAllAsset()const;
		SPtr<T>Register(const std::string& assetPath);
		void Register(SPtr<T> asset);
		void Register(std::vector<SPtr<T>>assets);
		void UnRegisterAllAsset();
	};
	template<typename T>
	inline bool AssetRegistry<T>::HasAsset(const std::string& name)const
	{
		if (Registry.find(name) != Registry.end())return true;
		return false;
	}

	template<typename T>
	inline SPtr<T> AssetRegistry<T>::GetAsset(const std::string& name) const
	{
		SPtr<T> ret;
		if (HasAsset(name))
		{
			ret = Registry.at(name);
		}
		return ret;
	}

	template<typename T>
	std::vector<SPtr<T>> AssetRegistry<T>::GetAllAsset() const
	{
		std::vector<SPtr<T>> ret(Registry.size());
		for (const auto& In : Registry)
		{
			ret.push_back(In.second);
		}
		return ret;
	}

	template<typename T>
	SPtr<T> AssetRegistry<T>::Register(const std::string& assetPath)
	{
		std::string name = Util::GetFileName(assetPath,false);
		if (HasAsset(name))return GetAsset(name);
		else
		{
			SPtr<T>Asset = T::Create(assetPath);
			Registry[name] = Asset;
			return Asset;
		}
		return SPtr<T>();
	}
	template<typename T>
	inline void AssetRegistry<T>::Register(SPtr<T>asset)
	{
		std::string name = asset->GetName();
		if (!HasAsset(name))
		{
			Registry[name] = asset;
		}
		return;
	}
	template<typename T>
	inline void AssetRegistry<T>::Register(std::vector<SPtr<T>>assets)
	{
		for (const auto& asset : assets)
		{
			Register(asset);
		}
	}
	template<typename T>
	inline void AssetRegistry<T>::UnRegisterAllAsset()
	{
		Registry.clear();
	}
}


