#pragma once
#include"Core/RHI/RHIBuffer.h"
#include<vulkan/vulkan.h>

namespace DM::RHI
{
	class VulkanDevice;
	class VulkanBuffer : public RHIBuffer
	{
		friend class VulkanPipeline;
	public:
		VulkanBuffer(VulkanDevice* device);
		~VulkanBuffer();

		/// <summary>
		/// 映射显存到 CPU 地址空间，返回可写指针。
		/// </summary>
		/// <returns></returns>
		void* Map() override;
		/// <summary>解除映射(连贯内存下数据已可见，无需显式 flush)。</summary>
		void  Unmap() override;
		uint32_t GetSize() const override { return m_Size; }

		virtual VkBuffer GetvkBuffer() const { return m_vkBuffer;}
		virtual void* GetMappedPtr()const { return m_MappedPtr;}

	protected:
		void CopyBuffer(VkBuffer src,VkBuffer dest);
	protected:
		VulkanDevice*const	m_Device;      
		VkBuffer			m_vkBuffer{};
		VkDeviceMemory		m_vkMemory{};
		void*				m_MappedPtr{};
		uint32_t			m_Size{};
	};

} // namespace DM::RHI
