#pragma once
#include"AssetPack.h"

namespace DM
{
	class Texture2D;
	struct DM_API TexturePack :public AssetPack
	{
		virtual EAssetType GetAssetType() const override { return EAssetType::Texture2D; }

		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		std::vector<uint8_t>m_Data;
	};
	REGISTER_CHILD(AssetPack, TexturePack);

	REGISTER_MEMBER(TexturePack, m_Width);
	REGISTER_MEMBER(TexturePack, m_Height);
	REGISTER_MEMBER(TexturePack, m_Data);

}