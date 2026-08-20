#pragma once
#include"../../RHITexture.h"
#include<vulkan/vulkan.h>
namespace DM::RHI
{
	class VulkanDevice;
	class VulkanTexture:public RHITexture
	{
	public:
		VulkanTexture(VulkanDevice*device, const RHITextureDesc& desc, const void* data);
		~VulkanTexture();
		VkImageView GetvkImageView()const { return m_vkImageView; }
		VkSampler GetvkSampler()const { return m_vkSampler; }
		VkImageLayout GetvkImageLayout()const { return m_vkImageLayout; }

	protected:
		void CreatevkImage(const RHITextureDesc& desc, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	private:
		VulkanDevice*	m_Device;
		VkImage			m_vkImage;
		VkImageView		m_vkImageView;
		VkDeviceMemory	m_vkImageMemory;
		VkImageLayout	m_vkImageLayout;
		VkSampler		m_vkSampler;

		VkDeviceSize m_BufferSize=0;

	};
}