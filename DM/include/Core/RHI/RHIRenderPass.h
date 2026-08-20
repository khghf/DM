#pragma once
#include"RHIResource.h"
namespace DM::RHI
{
	class RHIFramebuffer;
	class DM_API RHIRenderPass:public RHIResource
	{
	public:
		RHIRenderPass()=default;
		virtual ~RHIRenderPass()=default;
		virtual EResourceType GetAssetType() const override { return EResourceType::RenderPass; };

		virtual RHIFramebuffer* CreateFramebuffer(const RHIFramebufferDesc& desc) = 0;
	private:

	};

}