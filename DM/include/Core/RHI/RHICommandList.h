#pragma once
#include"Core/RHI/RHIResource.h"
namespace DM::RHI
{
	class RHIPipeline;
	class RHIBuffer;
	class RHISwapchain; 
	class RHIRenderPass;
	class RHIFramebuffer;
	class RHIDescriptorSetGroup;

	class DM_API RHICommandList : public RHIResource
	{
	public:
		virtual ~RHICommandList() = default;

		virtual void BeginRecord() = 0;
		virtual void EndRecord() = 0;

		virtual void BeginRenderPass(RHIRenderPass*renderPass, RHIFramebuffer*framebuffer) = 0;

		virtual void EndRenderPass() = 0;

		virtual void BindPipeline(RHIPipeline* pipeline) = 0;

		virtual void BindDescriptorSetGroup(RHIShaderProgram*shaderProgram,RHIDescriptorSetGroup* group) = 0;



		virtual void BindVertexBuffer(RHIBuffer* buffer,uint32_t binding = 0) = 0;

		virtual void BindIndexBuffer(RHIBuffer* buffer) = 0;

		virtual void SetViewport(uint32_t width, uint32_t height)=0;


		virtual void Draw(uint32_t vertexCount, uint32_t instanceCount = 1) = 0;

		virtual void DrawIndex(uint32_t indexCount, uint32_t instanceCount = 1) = 0;

		virtual void Submit()=0;

		
		EResourceType GetAssetType() const override { return EResourceType::CommandList; }

	protected:
		RHICommandList() = default;
	};

} // namespace DM::RHI
