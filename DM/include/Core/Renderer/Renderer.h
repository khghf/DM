#pragma once
#include<Core/RHI/RHI.h>
namespace DM
{
	class Renderer
	{
	public:
		Renderer()=default;
	public:
		virtual ~Renderer()=default;

		virtual void BeginFrame() { RHI::RHIDevice::Get()->BeginFrame(); };
		virtual void UpdateData(float deltaTime)=0;
		virtual void EndFrame() { RHI::RHIDevice::Get()->EndFrame(); };


		virtual RHI::RHIFramebuffer*	GetFramebuffer()const = 0;
		virtual RHI::RHISwapchain*		GetSwapchain()const = 0;
		virtual RHI::RHIRenderPass*		GetRenderPass()const = 0;
		virtual RHI::RHIPipeline*		GetPipeline()const = 0;
		virtual RHI::RHICommandList*	GetCommandList()const = 0;

	private:
	};
}