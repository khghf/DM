#pragma once
#include<Resource/Asset.h>
#include<Core/Core.h>
#include<string_view>
#include<Core/MMM/Reference.h>
namespace DM
{
	class DM_API Texture :public Asset
	{
	public:
		virtual ~Texture() = default;
		virtual void Bind(uint32_t unit=0) = 0;
		virtual uint32_t GetWidth() const= 0;
		virtual uint32_t GetHeight() const = 0;
		virtual void SetData(void*data,uint32_t size)= 0;
		virtual bool operator==(const Texture& other)const = 0;
	public:
		constexpr static EAssetsCategory s_AssetsCategory = EAssetsCategory::Texture;
		constexpr static std::string_view s_FileExtensionName = "tex";
	};
	class DM_API Texture2D :public Texture
	{
	public:
		virtual uint32_t GetId() { return 0; };
		constexpr static EAssetsCategory s_AssetsCategory = EAssetsCategory::Texture2D;
		static SPtr<Texture2D>Create(const std::string_view& path);
		static SPtr<Texture2D>Create(uint32_t width,uint32_t height);
	};
}
