#pragma once
#include<Core/AssetManagent/AssetObject/AssetObject.h>
namespace DM
{
	/// <summary>
	/// AssetObject 的 setter：负责写入资产对象的受保护成员(如 m_AssetId)。
	/// 与 AssetPackSetter 职责对应：pack 侧写 m_GUID，对象侧写 m_AssetId。
	/// </summary>
	struct DM_API AssetObjectSetter
	{
		static void SetAssetId(AssetObject* obj, const AssetID& assetId)
		{
			if (obj == nullptr)return;
			obj->m_AssetId = assetId;
		}
	};
}
