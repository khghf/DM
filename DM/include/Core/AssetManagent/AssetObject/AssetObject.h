#pragma once
#include"../AsetPack/AssetPack.h"
#include<Core/Object.h>
namespace DM
{
	class DM_API AssetObject:public Object
	{
		friend struct AssetObjectSetter;
	public:
		AssetObject()=default;
		~AssetObject();

		AssetID GetAssetId()const { return m_AssetId; }

		virtual EAssetType GetResourceType()const = 0;

		static EAssetType GetAssetType_Static() { return EAssetType::Unknown; }

		virtual std::string GetExtension() { return ".dasset";}

	protected:
		AssetID m_AssetId;
	};
}