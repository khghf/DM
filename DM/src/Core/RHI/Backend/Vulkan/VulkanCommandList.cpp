
#include<Core/RHI/Backend/Vulkan/VulkanCommandList.h>
#include<Core/RHI/Backend/Vulkan/VulkanDevice.h>
#include<Core/RHI/Backend/Vulkan/VulkanSwapchain.h>
#include<Core/RHI/Backend/Vulkan/VulkanPipeline.h>
#include<Core/RHI/Backend/Vulkan/VulkanShaderProgram.h>
#include<Core/RHI/Backend/Vulkan/Buffer/VulkanBuffer.h>
#include<Core/RHI/Backend/Vulkan/Buffer/VulkanVertexBuffer.h>
#include<Core/RHI/Backend/Vulkan/Buffer/VulkanIndexBuffer.h>
#include<Core/RHI/Backend/Vulkan/VulkanRenderPass.h>
#include<Core/RHI/Backend/Vulkan/VulkanFramebuffer.h>
#include<Core/RHI/Backend/Vulkan/VulkanDescriptorSetGroup.h>
namespace DM::RHI
{
	VulkanCommandList::VulkanCommandList(VulkanDevice* device)
		: m_Device(device)
	{
        //创建命令池
        VkCommandPoolCreateInfo poolCI{};
        poolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolCI.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        //  命令池标志，决定命令缓冲区的行为
        // 可用标志：
        // VK_COMMAND_POOL_CREATE_TRANSIENT_BIT 
        //    命令缓冲区短期使用(每帧重新录制) 
        //    提示驱动优化内存分配策略                                
        // 
        // VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
        //    允许单个命令缓冲区独立重置
        //    vkResetCommandBuffer() 可以单独重置
        //    如果不设置，只能 vkResetCommandPool() 重置整个池        
        // VK_COMMAND_POOL_CREATE_PROTECTED_BIT                         
        //    用于受保护内容（DRM/加密内容） 

        poolCI.queueFamilyIndex = m_Device->GetvkGraphicsQueueFamily(); // 命令缓冲从"图形族"的池分配
        // 绑定到图形队列族
        // 如果使用多个队列族(图形+传输)，需要为每个族创建独立的命令池
        // 例如：
        // - m_GraphicsCommandPool	(绑定到 GraphicsFamily)
        // - m_TransferCommandPool  (绑定到 TransferFamily)
        VK_CHECK(vkCreateCommandPool(m_Device->GetvkDevice(), &poolCI, nullptr, &m_CommandPool));
		// 从命令池分配1条 PRIMARY 缓冲(可单独提交)。
        m_CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo ai{};
		ai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		ai.commandPool = m_CommandPool;
		ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		ai.commandBufferCount = m_CommandBuffers.size();
        VK_CHECK(vkAllocateCommandBuffers(m_Device->GetvkDevice(), &ai, m_CommandBuffers.data()));
	}

	VulkanCommandList::~VulkanCommandList()
	{
        vkDestroyCommandPool(m_Device->GetvkDevice(), m_CommandPool, nullptr);
	}

    void VulkanCommandList::BeginRecord()
    {
        VkCommandBufferBeginInfo bi{};
        bi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        VK_CHECK(vkBeginCommandBuffer(*GetIdleCommandBuffer(), &bi));
    }

    void VulkanCommandList::EndRecord()
    {
        VK_CHECK(vkEndCommandBuffer(*GetIdleCommandBuffer()));
    }

    void VulkanCommandList::BeginRenderPass(RHIRenderPass* renderPass, RHIFramebuffer* framebuffer)
    {
        VulkanRenderPass* vulkanRenderPass = static_cast<VulkanRenderPass*>(renderPass);
        VulkanFramebuffer* vulkanFramebuffer = static_cast<VulkanFramebuffer*>(framebuffer);

        //m_Swapchain = static_cast<VulkanSwapchain*>(swapchain);
        //uint32_t imageIndex = m_Swapchain->AcquireNextImage();
        // ============================================================
        //开始录制命令缓冲区
        // ============================================================
        // VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT 标志说明：
        // 这个命令缓冲区只提交一次，之后就会被重置
        // 每帧都重新录制，适合动态场景
        // 
        // 其他可能的标志：
        // VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT：可同时多次提交
        // VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT：用于 Secondary 缓冲
        // ============================================================
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
        clearValues[1].depthStencil = { 1.0f, 0 };
        VkRenderPassBeginInfo rp{};
        rp.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

        rp.renderPass = vulkanRenderPass->GetvkRenderPass();

        rp.framebuffer = vulkanFramebuffer->GetvkFramebuffer();

        rp.renderArea.offset = { 0, 0 };
        rp.renderArea.extent.width = framebuffer->GetWidth();
        rp.renderArea.extent.height = framebuffer->GetHeight();

        rp.clearValueCount = clearValues.size();
        rp.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(*GetIdleCommandBuffer(), &rp, VK_SUBPASS_CONTENTS_INLINE);
    }

	void VulkanCommandList::EndRenderPass()
	{
		vkCmdEndRenderPass(*GetIdleCommandBuffer());       // 结束渲染通道
	}

	void VulkanCommandList::BindPipeline(RHIPipeline* pipeline)
	{
        VulkanPipeline* vkpipeline = static_cast<VulkanPipeline*>(pipeline);
        VkPipeline pl = vkpipeline->GetPipeline();
        VulkanShaderProgram* shaderProgram = static_cast<VulkanShaderProgram*>(vkpipeline->GetShaderProgram());
		vkCmdBindPipeline(*GetIdleCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pl);


	}

    void VulkanCommandList::BindDescriptorSetGroup(RHIShaderProgram* shaderProgram, RHIDescriptorSetGroup* group)
    {
        VulkanShaderProgram* vkShaderProgram = static_cast<VulkanShaderProgram*>(shaderProgram);
        VulkanDescriptorSetGroup* vkGroup = static_cast<VulkanDescriptorSetGroup*>(group);

        std::vector<VkDescriptorSet>vkSets = vkGroup->GetvkDescriptorSets();

        vkCmdBindDescriptorSets(*GetIdleCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, vkShaderProgram->m_vkPipelineLayout, 0, vkSets.size(), vkSets.data(), 0, nullptr);
    }

  

    void VulkanCommandList::BindVertexBuffer(RHIBuffer* buffer,uint32_t binding)
    {
        VulkanVertexBuffer* vbuf = static_cast<VulkanVertexBuffer*>(buffer);
        VkBuffer buf = vbuf->GetvkBuffer();
        VkDeviceSize offset = 0;
        vkCmdBindVertexBuffers(*GetIdleCommandBuffer(), binding, 1, &buf, &offset);
        //VKAPI_ATTR void VKAPI_CALL vkCmdBindVertexBuffers(
        //    VkCommandBuffer      commandBuffer,   // 命令缓冲区
        //    uint32_t             firstBinding,    // 起始绑定槽位
        //    uint32_t             bindingCount,    // 绑定多少个缓冲区
        //    const VkBuffer * pBuffers,            // 缓冲区句柄数组
        //    const VkDeviceSize * pOffsets         // 每个缓冲区的偏移量数组
        //);

    }

    void VulkanCommandList::BindIndexBuffer(RHIBuffer* buffer)
    {
        VulkanIndexBuffer* vbuf = static_cast<VulkanIndexBuffer*>(buffer);
        VkBuffer buf = vbuf->GetvkBuffer();
        VkDeviceSize offset = 0;
        vkCmdBindIndexBuffer(*GetIdleCommandBuffer(), buf, 0, VK_INDEX_TYPE_UINT32);
    }

    void VulkanCommandList::SetViewport(uint32_t width, uint32_t height)
    {
        VkViewport viewport;
        viewport.x = 0.0f;
        viewport.y = height;
        viewport.width = (float)width;
        viewport.height = -(float)height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(*GetIdleCommandBuffer(), 0, 1, &viewport);

        VkRect2D scissor;
        scissor.offset = { 0, 0 };
        scissor.extent.width = width;
        scissor.extent.height = height;
        vkCmdSetScissor(*GetIdleCommandBuffer(), 0, 1, &scissor);
    }

	void VulkanCommandList::Draw(uint32_t vertexCount, uint32_t instanceCount)
	{
        vkCmdDraw(*GetIdleCommandBuffer(), vertexCount, instanceCount, 0, 0);
	}

    void VulkanCommandList::DrawIndex(uint32_t indexCount, uint32_t instanceCount)
    {
        vkCmdDrawIndexed(*GetIdleCommandBuffer(), indexCount, instanceCount, 0, 0, 0);
        //VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexed(
        //    VkCommandBuffer  commandBuffer,   // 命令缓冲区
        //    uint32_t         indexCount,      // 要绘制的索引数量
        //    uint32_t         instanceCount,   // 实例数量（实例化渲染）
        //    uint32_t         firstIndex,      // 索引缓冲区的起始位置
        //    int32_t          vertexOffset,    // 从索引缓冲区读取的每个索引值，都加上 vertexOffset 后再去顶点缓冲区取顶点。
        //    uint32_t         firstInstance    // 实例化起始索引
        //);
    }

    void VulkanCommandList::Submit()
    {
        // 提交要"等图像可用后再开始画"，画完"举起渲染完成信号量"。
        VkSemaphore waitSemaphores[] = { m_Device->GetImageAvailableSemaphore() };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }; // 等到"能写颜色附件"阶段
        VkSemaphore signalSemaphores[] = { m_Device->GetRenderFinishedSemaphore() };

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = GetIdleCommandBuffer();

        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        // fence 不为空时把它交给本次提交：GPU 跑完会自动"举起"这个 fence，CPU 之后可 Wait 它。
        VkFence fenceHandle = *m_Device->GetInFlightFence();
        VK_CHECK(vkQueueSubmit(m_Device->GetvkGraphicsQueue(), 1, &submitInfo, fenceHandle));
    }
    VkCommandBuffer* VulkanCommandList::GetIdleCommandBuffer() const
    {
        return const_cast<VkCommandBuffer*>(&m_CommandBuffers[CURRENT_CPU_PROCESSES_FRAME_INDEX]);
    }
} // namespace DM::RHI
