#pragma once
#include"AssetObject.h"
namespace DM
{
	namespace RHI
	{
		class RHIShader;
	}

	class DM_API Shader:public AssetObject
	{
		friend class ShaderLoader;
	public:
		~Shader();
		virtual EAssetType GetAssetType()const override { return GetAssetType_Static(); }
		static EAssetType GetAssetType_Static() { return EAssetType::Unknown; }

		RHI::RHIShader* GetRHIResource()const { return m_RHISHader; }

	private:
		RHI::RHIShader* m_RHISHader{};
	};
}