#include<Core/RHI/Backend/Vulkan/VulkanTexture.h>
#include<Core/RHI/Backend/Vulkan/VulkanDevice.h>
#include<Core/RHI/Backend/Vulkan/VulkanTypeMapping.h>
#include<stb_image.h>
namespace DM::RHI
{
	static uint8_t ToSTBFormat(ERHIFormat rhiFormat)
	{
		switch (rhiFormat)
		{
		case DM::RHI::ERHIFormat::R8_UNorm:
			return STBI_grey;
			break;
		case DM::RHI::ERHIFormat::R8G8B8_UNorm:
			return STBI_rgb;
			break;
		case DM::RHI::ERHIFormat::R8G8B8A8_UNorm:
			return STBI_rgb_alpha;
			break;
		case DM::RHI::ERHIFormat::B8G8R8A8_UNorm:
			return STBI_rgb_alpha;
		default:
			return 0;
			break;
		}
	}
	VulkanTexture::VulkanTexture(VulkanDevice* device, const RHITextureDesc& desc, const void* data):m_Device(device)
	{
		m_BufferSize = FormatByteSize(desc.Format) * desc.Width * desc.Height;

		CreatevkImage(desc, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vkImage, m_vkImageMemory);

		if (data !=nullptr)
		{
			//创建中间缓冲区暂存图像数据
			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;
			device->CreatevkBuffer(m_BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
			void* mapPtr;
			vkMapMemory(device->GetvkDevice(), stagingBufferMemory, 0, m_BufferSize, 0, &mapPtr);
			memcpy(mapPtr, data, m_BufferSize);
			vkUnmapMemory(device->GetvkDevice(), stagingBufferMemory);


			//转换布局准备填充数据
			m_Device->TransitionImageLayout(m_vkImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			m_Device->CopyvkBufferTovkImage(stagingBuffer, m_vkImage, static_cast<uint32_t>(desc.Width), static_cast<uint32_t>(desc.Height));
			//转换布局供着色器读取


			m_vkImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			m_Device->TransitionImageLayout(m_vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_vkImageLayout);



			vkDestroyBuffer(m_Device->GetvkDevice(), stagingBuffer, nullptr);
			vkFreeMemory(m_Device->GetvkDevice(), stagingBufferMemory, nullptr);
		}

		//创建图像视图
		m_Device->CreatevkImageView(m_vkImage, ToVkImageViewType(desc.Type), ToVkFormat(desc.Format), VK_IMAGE_ASPECT_COLOR_BIT, m_vkImageView);

		//创建采样器
		// ---- 1. 获取设备属性(用于各向异性) ----
		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(m_Device->GetvkPhysicalDevice(), &properties);

		// ---- 2. 配置采样器 ----
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

		// ---- 2.1 过滤模式 ----
		samplerInfo.magFilter = VK_FILTER_LINEAR;   // 放大：线性插值
		samplerInfo.minFilter = VK_FILTER_LINEAR;   // 缩小：线性插值
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

		// ---- 2.2 寻址模式 ----
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		// ---- 2.3 各向异性过滤 ----
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

		// ---- 2.4 边界颜色 ----
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

		// ---- 2.5 坐标系统 ----
		samplerInfo.unnormalizedCoordinates = VK_FALSE;  // 使用 [0,1) 归一化坐标

		// ---- 2.6 比较函数(阴影贴图用) ----
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

		// ---- 2.7 Mipmap 控制 ----
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;  // 0 = 只使用最高 Mip
		samplerInfo.mipLodBias = 0.0f;

		// ---- 3. 创建采样器 ----
		VK_CHECK(vkCreateSampler(m_Device->GetvkDevice(), &samplerInfo, nullptr, &m_vkSampler));

	}
	
	VulkanTexture::~VulkanTexture()
	{
		vkDestroySampler(m_Device->GetvkDevice(), m_vkSampler, nullptr);
		vkDestroyImageView(m_Device->GetvkDevice(), m_vkImageView, nullptr);
		vkDestroyImage(m_Device->GetvkDevice(), m_vkImage, nullptr);
		vkFreeMemory(m_Device->GetvkDevice(), m_vkImageMemory, nullptr);
	}
	

	void VulkanTexture::CreatevkImage(const RHITextureDesc& desc, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
	{
		m_Device->CreatevkImage(ToVkImageType(desc.Type), desc.Width, desc.Height, ToVkFormat(desc.Format),VK_SAMPLE_COUNT_1_BIT,tiling, usage, properties, m_vkImage, m_vkImageMemory, desc.Depth, desc.MipLevels, desc.ArrayLayers);
	}
}