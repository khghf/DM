#pragma once
#include"RHITypes.h"
#include"RHIResource.h"
#include"RHIDevice.h"
#include"RHIRenderPass.h"
#include"RHIFramebuffer.h"
#include"RHISwapchain.h"
#include"RHIBuffer.h"
#include"RHIShader.h"
#include"RHIShaderProgram.h"
#include"RHIPipeline.h"
#include"RHICommandList.h"
#include"RHITexture.h"
#include"RHIDescriptorSet.h"
#include"RHIDescriptorSetGroup.h"
namespace DM::RHI
{
	inline DM_API RHIDevice* GetDevice()
	{
		return RHIDevice::Get();
	}
}