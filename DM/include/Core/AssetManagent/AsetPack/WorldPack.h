#pragma once
#include"AssetPack.h"
namespace DM
{
	struct DM_API WorldPack:AssetPack
	{
		virtual EAssetType GetResourceType() const override { return EAssetType::World; }
		virtual std::string GetExtension() const override { return ".world"; }

		std::string m_WorldName;
	};
	REGISTER_CHILD(AssetPack, WorldPack);
	REGISTER_MEMBER(WorldPack, m_WorldName);
}