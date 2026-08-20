#pragma once
#include"Core/RHI/RHICommandList.h"
#include<vulkan/vulkan.h>

namespace DM::RHI
{
	class VulkanDevice;
	class VulkanSwapchain;
	class VulkanPipeline;
	class VulkanBuffer;
	extern uint8_t CURRENT_CPU_PROCESSES_FRAME_INDEX;

	/// <summary>Vulkan 命令列表实现。</summary>
	class DM_API VulkanCommandList : public RHICommandList
	{
	public:
		VulkanCommandList(VulkanDevice* device);
		virtual ~VulkanCommandList() override;

		virtual void BeginRecord()override;
		virtual void EndRecord()override;
		virtual void BeginRenderPass(RHIRenderPass* renderPass, RHIFramebuffer* framebuffer) override;
		virtual void EndRenderPass() override;

		virtual void BindPipeline(RHIPipeline* pipeline) override;

		virtual void BindDescriptorSetGroup(RHIShaderProgram* shaderProgram,RHIDescriptorSetGroup* group) override;



		virtual void BindVertexBuffer(RHIBuffer* buffer,uint32_t binding) override;
		virtual void BindIndexBuffer(RHIBuffer* buffer) override;
		virtual void SetViewport(uint32_t width, uint32_t height) override;
		virtual void Draw(uint32_t vertexCount, uint32_t instanceCount) override;
		virtual void DrawIndex(uint32_t indexCount, uint32_t instanceCount) override;
		virtual void Submit()override;
		/// <summary>
		/// 获取一个空闲的命令缓冲区。
		/// </summary>
		/// <returns>返回一个空闲的 VkCommandBuffer 对象。</returns>
		VkCommandBuffer* GetIdleCommandBuffer() const;

	private:
		// ---- 成员变量 ----
		VulkanDevice*    m_Device;        // 所属设备（借引用）
		VkCommandPool    m_CommandPool;   // 这条缓冲是从哪个池子分配的（销毁时要回池子里释放）
		std::vector<VkCommandBuffer>  m_CommandBuffers; // 真正的录制缓冲（所有 vkCmd* 都写在它里面）
		//VulkanSwapchain* m_Swapchain;     // 当前帧的目标交换链（Begin 时记下，用来开 RenderPass）
	};

} // namespace DM::RHI
