#pragma once
#include"AssetObject.h"

namespace DM
{
	namespace RHI
	{
		class RHIBuffer;
	}
	class Texture2D;
	class DM_API Model:public AssetObject
	{
		friend class ModelLoader;
	public:
		Model();
		~Model();
		virtual EAssetType GetAssetType()const override { return GetAssetType_Static(); };
		static EAssetType GetAssetType_Static() { return EAssetType::Model; }
	private:
		RHI::RHIBuffer* m_Vertices=nullptr;
		RHI::RHIBuffer* m_Indices= nullptr;

		std::vector<SPtr<AssetObject>>m_RefedTexture;
	};
	REGISTER_CHILD(AssetObject, Model);
	
}