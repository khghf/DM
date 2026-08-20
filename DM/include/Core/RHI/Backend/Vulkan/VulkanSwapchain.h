#pragma once
#include"Core/RHI/RHISwapchain.h"
#include<vulkan/vulkan.h>
#include<vector>
#include<Core/RHI/Backend/Vulkan/VulkanTypeMapping.h>
#include<Core/RHI/RHITypes.h>

namespace DM::RHI
{
	class VulkanDevice;

	class VulkanSwapchain : public RHISwapchain
	{
	public:
		VulkanSwapchain(VulkanDevice* device, const RHISwapchainDesc& desc);
		~VulkanSwapchain() override;
		// ---- RHI 接口实现 ----
		virtual void		GetExtent(uint32_t& width, uint32_t& height) const override { width = m_vkExtent.width; height = m_vkExtent.height; }
		virtual uint32_t	GetCurrentImageIndex() const override { return m_CurrentIndex; }
		virtual uint32_t	GetImageCount()const override { return m_ImageCount; }
		virtual ERHIFormat	GetFormat()const override { return ToRHIFormat(m_vkSwapchainFormat); }

		virtual void		Present()override;
		virtual uint32_t	AcquireNextImage() override;

		void					Submit(VkCommandBuffer* cmd);
		VkSwapchainKHR			GetvkSwapchain()   const { return m_vkSwapchain; }
		VkFormat				GetvkSwapchainImageFormat() const { return m_vkSwapchainFormat; }
		VkExtent2D				GetvkExtent()    const { return m_vkExtent; }
		VkSampleCountFlagBits	GetvkSampleCount() const{ return m_vkSampleCount; }
		uint32_t				GetMinImageCount()const { return m_MinImageCount; }
	private:
		void CreatevkSwapchain();
		void DestroyvkSwapchain();
		VkImage GetCurrentImage()const { return m_vkSwapchainImages[GetCurrentImageIndex()]; }
		/// <summary>
		/// 从给定的模式列表中选择交换链的呈现模式。
		/// </summary>
		/// <param name="modes">一个包含可用呈现模式的 VkPresentModeKHR 类型的向量。</param>
		/// <returns>选择的 VkPresentModeKHR 类型的呈现模式。</returns>
		static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& modes);
		/// <summary>
		/// 获取交换链的分辨率范围。
		/// </summary>
		/// <param name="caps">VkSurfaceCapabilitiesKHR 类型的结构，描述表面支持的能力。</param>
		/// <param name="windowHandle">指向窗口句柄的指针，用于获取帧缓冲区的大小。</param>
		/// <returns>VkExtent2D 类型的结构，表示交换链的分辨率范围。</returns>
		static VkExtent2D GetSwapExtent(const VkSurfaceCapabilitiesKHR& caps, void* windowHandle);
		/// <summary>
		/// 重新创建交换链。
		/// </summary>
		void RecreateSwapChain();
		

	private:
		VulkanDevice*				m_Device;                              
		VkSwapchainKHR				m_vkSwapchain;  

		std::vector<VkImage>		m_vkSwapchainImages;
		std::vector<VkImageView>	m_vkSwapchainImageViews;

		VkFormat					m_vkSwapchainFormat;
		VkExtent2D					m_vkExtent{};
		VkSampleCountFlagBits		m_vkSampleCount = VK_SAMPLE_COUNT_1_BIT;
		uint32_t					m_CurrentIndex;                  
		RHISwapchainDesc			m_Desc;
		uint32_t					m_MinImageCount;
		uint32_t					m_ImageCount;
	
	};

} // namespace DM::RHI
