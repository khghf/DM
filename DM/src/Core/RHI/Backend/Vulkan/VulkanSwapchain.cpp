#include<Core/RHI/Backend/Vulkan/VulkanSwapchain.h>
#include<Core/RHI/Backend/Vulkan/VulkanFramebuffer.h>
#include<Core/RHI/Backend/Vulkan/VulkanDevice.h>
#include<Core/RHI/RHIDevice.h>
#include<stdexcept>
#include<GLFW/glfw3.h>

namespace DM::RHI
{
	
	VulkanSwapchain::VulkanSwapchain(VulkanDevice* device, const RHISwapchainDesc& desc)
		: m_Device(device),m_Desc(desc)
	{
		VkSampleCountFlagBits maxUsableSampleCount = m_Device->GetMaxUsableSampleCount();
		VkSampleCountFlagBits requiredSampleCount= ToVkSampleCount(desc.SampleMode);
		if (requiredSampleCount > maxUsableSampleCount)
		{
			m_vkSampleCount = maxUsableSampleCount;
			std::cout << "Required smaple count is bigger than max usable smaple count:"<<std::to_string(maxUsableSampleCount)<<"\n";
		}
		else
		{
			m_vkSampleCount = requiredSampleCount;
		}

		CreatevkSwapchain();
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		DestroyvkSwapchain();
	}

	void VulkanSwapchain::CreatevkSwapchain()
	{
		VkPhysicalDevice gpu = m_Device->GetvkPhysicalDevice();
		VkSurfaceKHR surface = m_Device->GetvkSurface();

		// 查询交换链支持的格式/呈现模式/尺寸
		SwapChainSupportDetails swapChainSupport = m_Device->QueryvkSwapChainSupport(gpu);

		// 选择一个合适的表面格式
		VkSurfaceFormatKHR surfaceFormat = m_Device->ChooseSwapSurfaceFormat(swapChainSupport.formats);

		// 选择合适的呈现模式优先MAILBOX(三重缓冲)
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);


		m_vkSwapchainFormat = surfaceFormat.format;
		m_vkExtent = GetSwapExtent(swapChainSupport.capabilities, m_Desc.WindowHandle);


		m_MinImageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && m_MinImageCount > swapChainSupport.capabilities.maxImageCount)
		{
			m_MinImageCount = swapChainSupport.capabilities.maxImageCount;
		}

		//创建交换链
		VkSwapchainCreateInfoKHR ci{};
		ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		ci.surface = surface;
		ci.minImageCount = m_MinImageCount;// 缓冲区数量
		ci.imageFormat = surfaceFormat.format;// 像素格式(BGRA_SRGB)
		ci.imageColorSpace = surfaceFormat.colorSpace; //颜色空间(sRGB)
		ci.imageExtent = m_vkExtent;// 分辨率(像素单位)
		ci.imageArrayLayers = 1; // 图像层数(立体视觉用，通常为1)
		ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT| VK_IMAGE_USAGE_TRANSFER_DST_BIT; // 这些图用来当"颜色附件"(被画上去)

		// 图形队列族与呈现队列族可能相同；相同用 EXCLUSIVE(独占，更高效)，
		// 不同则需要 CONCURRENT(并发)并列出两个族，否则跨族访问会出错。
		uint32_t g = m_Device->GetvkGraphicsQueueFamily();
		uint32_t p = m_Device->GetvkPresentQueueFamily();
		uint32_t queueFamilies[] = { g, p };
		if (g != p)
		{
			ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			ci.queueFamilyIndexCount = 2;
			ci.pQueueFamilyIndices = queueFamilies;
		}
		else
		{
			ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		ci.preTransform = swapChainSupport.capabilities.currentTransform;            // 不额外旋转
		ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // 不透明合成
		ci.presentMode = presentMode;
		ci.clipped = VK_TRUE;          // 被窗口遮挡的部分可以不画(优化)
		ci.oldSwapchain = VK_NULL_HANDLE; // 重建时填旧的；这里首次创建为 null

		VK_CHECK(vkCreateSwapchainKHR(m_Device->GetvkDevice(), &ci, nullptr, &m_vkSwapchain));

		//取出交换链内部那几张图像(VkImage)
		vkGetSwapchainImagesKHR(m_Device->GetvkDevice(), m_vkSwapchain, &m_ImageCount, nullptr);
		m_vkSwapchainImages.resize(m_ImageCount);
		vkGetSwapchainImagesKHR(m_Device->GetvkDevice(), m_vkSwapchain, &m_ImageCount, m_vkSwapchainImages.data());
		//创建视图
		m_vkSwapchainImageViews.resize(m_vkSwapchainImages.size());
		for (size_t i = 0; i < m_vkSwapchainImages.size(); ++i)
		{
			m_Device->CreatevkImageView(m_vkSwapchainImages[i], VK_IMAGE_VIEW_TYPE_2D, m_vkSwapchainFormat, VK_IMAGE_ASPECT_COLOR_BIT, m_vkSwapchainImageViews[i]);
		}
	}

	void VulkanSwapchain::DestroyvkSwapchain()
	{
		for (auto iv : m_vkSwapchainImageViews) vkDestroyImageView(m_Device->GetvkDevice(), iv, nullptr);
		if (m_vkSwapchain) vkDestroySwapchainKHR(m_Device->GetvkDevice(), m_vkSwapchain, nullptr);
	}
	
	

	VkPresentModeKHR VulkanSwapchain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& modes)
	{
		// MAILBOX = 三重缓冲(低延迟，新帧覆盖旧帧)；退而求其次 FIFO(垂直同步，绝不撕裂)。
		for (const auto& m : modes)if (m == VK_PRESENT_MODE_MAILBOX_KHR) return m;

		//typedef enum VkPresentModeKHR {
		//	// 立即模式：渲染完就显示，不等垂直同步信号
		//	// ============================================================
		//	// 优点：延迟最低(渲染完立刻上屏)
		//	// 缺点：有画面撕裂(屏幕刷新到一半时切换缓冲)
		//	// 注：需要检查设备是否支持
		//	VK_PRESENT_MODE_IMMEDIATE_KHR = 0,


		//	// 邮箱模式：三重缓冲 + 低延迟 VSync
		//	// ============================================================
		//	// 优点：无撕裂 + 低延迟(始终显示最新帧)
		//	// 缺点：功耗较高(GPU 可能跑满)
		//	// 注：需要检查设备是否支持
		//	VK_PRESENT_MODE_MAILBOX_KHR = 1,


		//	// 队列模式：标准垂直同步(VSync)
		//	// ============================================================
		//	// 优点：无撕裂
		//	// 缺点：延迟较高(排队等待)
		//	// 帧率：锁死在显示器刷新率(如 60 FPS)
		//	VK_PRESENT_MODE_FIFO_KHR = 2,


		//	// 宽松队列模式：帧率低时不等待
		//	// ============================================================
		//	// 优点：帧率低时减少延迟
		//	// 缺点：帧率低时可能有撕裂
		//	// 注：需要检查设备是否支持
		//	VK_PRESENT_MODE_FIFO_RELAXED_KHR = 3,


		//	// 共享按需刷新模式(VK_KHR_shared_presentable_image 扩展)
		//	// ============================================================
		//	// 说明：多个窗口共享同一个交换链，按需刷新
		//	// 场景：多窗口应用、VR 头显
		//	// 注：极少使用，需要检查设备和扩展支持
		//	// 数字 1000111000 = 扩展编码(Vulkan 扩展枚举值格式)
		//	VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR = 1000111000,


		//	// 共享持续刷新模式(VK_KHR_shared_presentable_image 扩展)
		//	// ============================================================
		//	// 说明：多个窗口共享同一个交换链，持续刷新
		//	// 场景：多窗口应用、VR 头显
		//	// 注：极少使用，需要检查设备和扩展支持
		//	VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR = 1000111001,


		//	// 枚举最大值(强制 32 位)
		//	// ============================================================
		//	// 说明：这是 C 枚举的技巧，确保枚举类型占用 4 字节
		//	// 用途：保证在不同编译器下大小一致
		//	// 值 0x7FFFFFFF = 32 位有符号整数的最大值
		//	VK_PRESENT_MODE_MAX_ENUM_KHR = 0x7FFFFFFF
		//} VkPresentModeKHR;
		return modes[0];
	}
	
	VkExtent2D VulkanSwapchain::GetSwapExtent(const VkSurfaceCapabilitiesKHR& caps, void* windowHandle)
	{
		if (caps.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
		{
			return caps.currentExtent;
		}
		else {
			int width, height;
			glfwGetFramebufferSize(static_cast<GLFWwindow*>(windowHandle), &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, caps.minImageExtent.width, caps.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, caps.minImageExtent.height, caps.maxImageExtent.height);

			return actualExtent;
		}
	}

	void VulkanSwapchain::RecreateSwapChain()
	{
		int width = 0, height = 0;
		glfwGetFramebufferSize(static_cast<GLFWwindow*>(m_Desc.WindowHandle), &width, &height);
		while (width == 0 || height == 0) 
		{
			glfwGetFramebufferSize(static_cast<GLFWwindow*>(m_Desc.WindowHandle), &width, &height);
			glfwWaitEvents();
		}
		m_Device->WaitGPUIdle();
		DestroyvkSwapchain();
		
		CreatevkSwapchain();
	}

	uint32_t VulkanSwapchain::AcquireNextImage()
	{
		// 每帧第一件事：问交换链"现在该画第几张图？"。
		// vkAcquireNextImageKHR 会等垂直同步(拿到一张可画的图)，并把 m_ImageAvailable 信号量举起，
		// 然后把"那张图的下标"写进 m_CurrentIndex 返回。之后的画都画到这张图上。
		VkResult r = vkAcquireNextImageKHR(m_Device->GetvkDevice(), m_vkSwapchain,UINT64_MAX, m_Device->GetImageAvailableSemaphore(), VK_NULL_HANDLE, &m_CurrentIndex);
		if (r == VK_ERROR_OUT_OF_DATE_KHR) 
		{
			RecreateSwapChain();
			AcquireNextImage();
		}
		else if (r != VK_SUCCESS && r != VK_SUBOPTIMAL_KHR) 
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}
		return m_CurrentIndex;
	}

	void VulkanSwapchain::Submit(VkCommandBuffer*cmd)
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
		submitInfo.pCommandBuffers = cmd;

		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		// fence 不为空时把它交给本次提交：GPU 跑完会自动"举起"这个 fence，CPU 之后可 Wait 它。
		VkFence fenceHandle = *m_Device->GetInFlightFence();
		VK_CHECK(vkQueueSubmit(m_Device->GetvkGraphicsQueue(), 1, &submitInfo, fenceHandle));

	}

	void VulkanSwapchain::Present()
	{
		assert(m_PresentTarget && "Forgot to setup present taregt");

		//将帧缓冲的渲染结果拷贝到交换链图像
		VulkanFramebuffer* vulkanFramebuffer = static_cast<VulkanFramebuffer*>(m_PresentTarget);
		m_PresentTarget = nullptr;
		VkImageCopy copyRegion{};
		copyRegion.srcOffset = { 0,0,0 };
		copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.srcSubresource.baseArrayLayer = 0;
		copyRegion.srcSubresource.layerCount = 1;
		copyRegion.srcSubresource.mipLevel = 0;

		copyRegion.dstOffset = { 0,0,0 };
		copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.dstSubresource.baseArrayLayer = 0;
		copyRegion.dstSubresource.layerCount = 1;
		copyRegion.dstSubresource.mipLevel = 0;

		copyRegion.extent.width = GetvkExtent().width;
		copyRegion.extent.height = GetvkExtent().height;
		copyRegion.extent.depth = 1;

		m_Device->TransitionImageLayout(GetCurrentImage(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		VkCommandBuffer cmdbuffer = m_Device->BeginSingleTimeCommands();
		vkCmdCopyImage(cmdbuffer, vulkanFramebuffer->GetColorAttachment().vkImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, GetCurrentImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
		m_Device->EndSingleTimeCommands();

		m_Device->TransitionImageLayout(GetCurrentImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		VulkanSwapchain* vsc = this;
		uint32_t imageIndex = vsc->GetCurrentImageIndex();

		// 等"渲染完成"信号量后，把那张图像推到屏幕。
		std::vector<VkSemaphore>waitSemaphores={ m_Device->GetRenderFinishedSemaphore()};
		std::vector<VkSwapchainKHR>swapChains= { vsc->GetvkSwapchain() };

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = waitSemaphores.size();
		presentInfo.pWaitSemaphores = waitSemaphores.data();
		presentInfo.swapchainCount = swapChains.size();
		presentInfo.pSwapchains = swapChains.data();
		presentInfo.pImageIndices = &imageIndex;

		VkResult result = vkQueuePresentKHR(m_Device->GetvkPresentQueue(), &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) 
		{
			RecreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain!");
		}
	}
} // namespace DM::RHI
