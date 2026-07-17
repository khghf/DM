#pragma once
#include<string>
namespace DM
{
	enum class EAssetsCategory :uint8_t//资源类型
	{
		Asset,
		Texture, Texture2D,
		Shader,
	};
	class Asset
	{
	public:
		Asset() = default;
		virtual ~Asset() = default;
		std::string GetName()const { return m_Name; };
		std::string GetPath()const { return m_AssetPath; };
		/*virtual void	SaveToLocal(void* data, size_t size) {}
		virtual void*	ReadFromLocal() {}*/
	public:
		constexpr static EAssetsCategory s_AssetsCategory = EAssetsCategory::Asset;
		constexpr static std::string_view s_FileExtensionName = "asset";
	protected:
		std::string m_Name;
		std::string m_AssetPath;
	};
}
