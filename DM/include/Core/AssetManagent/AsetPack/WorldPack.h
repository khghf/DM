#pragma once
#include"AssetPack.h"
#include <Framework/Base/WorldSerializer.h>
namespace DM
{
	/// <summary>
	/// World 资产包(自包含：包路径即源文件路径，无 .dasset 后缀)。
	/// m_WorldName 为场景名；m_Entities 承载实体快照(WorldSerializer 收集)。
	/// </summary>
	struct DM_API WorldPack:AssetPack
	{
		virtual EAssetType GetResourceType() const override { return EAssetType::World; }
		virtual std::string GetExtension() const override { return ".world"; }

		std::string m_WorldName;
		std::vector<EntitySnapshot> m_Entities;   // 实体快照(组件数据)
	};
	REGISTER_CHILD(AssetPack, WorldPack);
	REGISTER_MEMBER(WorldPack, m_WorldName);
	REGISTER_MEMBER(WorldPack, m_Entities);
}