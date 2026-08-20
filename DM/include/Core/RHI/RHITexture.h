#pragma once
#include"RHIResource.h"
namespace DM::RHI
{
	class DM_API RHITexture:public RHIResource
	{
	public:
		RHITexture()=default;
		~RHITexture()=default;

		virtual EResourceType GetAssetType() const { return EResourceType::Texture; }
	private:

	};
}