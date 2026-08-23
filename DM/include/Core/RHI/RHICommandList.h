#pragma once
#include"Core/RHI/RHIResource.h"
namespace DM::RHI
{
	class RHIPipeline;
	class RHIBuffer;
	class RHISwapchain; 
	class RHIRenderPass;
	class RHIFramebuffer;
	class RHIDescriptorSet;
	class RHIDescriptorSetGroup;
	/// <summary>
	/// [笔记]命令缓冲与命令池。对于命令的执行opengl使用的是即时模式调用即执行，vulkan则使用命令缓冲来记录命令然后统一提交给GPU执行，
	/// 命令池管理命令缓冲，命令池需要绑定一个队列之后所有分配出来的命令缓冲只能记录该队列能执行的命令
	/// </summary>
	class DM_API RHICommandList : public RHIResource
	{
	public:
		virtual ~RHICommandList() = default;

		virtual void BeginRecord() = 0;
		virtual void EndRecord() = 0;

		virtual void BeginRenderPass(RHIRenderPass*renderPass, RHIFramebuffer*framebuffer) = 0;

		virtual void EndRenderPass() = 0;

		virtual void BindPipeline(RHIPipeline* pipeline) = 0;

		virtual void BindDescriptorSet(RHIShaderProgram* shaderProgram, RHIDescriptorSet* set) = 0;
		virtual void BindDescriptorSetGroup(RHIShaderProgram*shaderProgram,RHIDescriptorSetGroup* group) = 0;



		virtual void BindVertexBuffer(RHIBuffer* buffer,uint32_t binding = 0) = 0;

		virtual void BindIndexBuffer(RHIBuffer* buffer) = 0;

		virtual void SetViewport(uint32_t width, uint32_t height)=0;


		virtual void Draw(uint32_t vertexCount, uint32_t instanceCount = 1) = 0;

		virtual void DrawIndex(uint32_t indexCount, uint32_t instanceCount = 1) = 0;

		virtual void Submit()=0;

		
		EResourceType GetResourceType() const override { return EResourceType::CommandList; }

	protected:
		RHICommandList() = default;
	};

} // namespace DM::RHI
