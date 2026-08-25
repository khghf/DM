#pragma once
#include"../../RHITexture.h"
#include<vulkan/vulkan.h>

// VMA 分配句柄前置声明（完整定义见 vk_mem_alloc.h，仅在实现文件中包含）
struct VmaAllocation_T;
typedef VmaAllocation_T* VmaAllocation;

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
		void CreatevkImage(const RHITextureDesc& desc, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VmaAllocation& imageAllocation);
	private:
		VulkanDevice*	m_Device;
		VkImage			m_vkImage;
		VkImageView		m_vkImageView;
		VmaAllocation	m_vmaAllocation;
		VkImageLayout	m_vkImageLayout;
		VkSampler		m_vkSampler;

		VkDeviceSize m_BufferSize=0;

	};
}