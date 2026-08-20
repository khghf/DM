#pragma once
#include"AssetObject.h"
namespace DM
{
	namespace RHI
	{
		class RHITexture;
	}
}

namespace DM
{
	class TexturePack;
	class DM_API Texture2D:public AssetObject
	{
		friend struct Texture2DLoader;
	public:
		Texture2D();
		~Texture2D();
		 RHI::RHITexture* GetRHIResource()const { return m_RHITexture; }

	private:
		Texture2D(const TexturePack*pack);

		virtual EAssetType GetAssetType()const override { return Texture2D::GetAssetType_Static(); }
		static EAssetType GetAssetType_Static() { return EAssetType::Texture2D; }
	private:
		uint32_t m_Width;
		uint32_t m_Height;
		 RHI::RHITexture* m_RHITexture=nullptr;
	};
}