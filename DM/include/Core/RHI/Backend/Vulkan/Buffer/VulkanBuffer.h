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
		VulkanBuffer(VulkanDevice* device,const RHIBufferDesc&desc);
		~VulkanBuffer();
		virtual void Update(const void* data, size_t size)override;

		/// <summary>
		/// 映射显存到 CPU 地址空间，返回可写指针。
		/// </summary>
		/// <returns></returns>
		void* Map() override;
		/// <summary>解除映射(连贯内存下数据已可见，无需显式 flush)。</summary>
		void  Unmap() override;
		uint32_t GetSize() const override { return m_Size; }


		virtual VkBuffer	GetvkBuffer() const { return m_vkBuffer;}
		virtual void*		GetMappedPtr()const { return m_MappedPtr;}

		virtual EResourceType GetResourceType()const override { return EResourceType::Buffer; };
	protected:
		void CopyBuffer(VkBuffer src,VkBuffer dest);

		void CreateVertexBuffer(const RHIBufferDesc& desc);
		void UpdateVertexBuffer(const void* data, size_t size);

		void CreateIndexBuffer(const RHIBufferDesc& desc);
		void UpdateIndexBuffer(const void* data, size_t size);

		void CreateUniformBuffer(const RHIBufferDesc& desc);
		void UpdateUniformBuffer(const void* data, size_t size);

		void CreateStorageBuffer(const RHIBufferDesc& desc);
		void UpdateStorageBuffer(const void* data, size_t size);
	protected:
		VulkanDevice*const	m_Device;      
		VkBuffer			m_vkBuffer{};
		VkDeviceMemory		m_vkMemory{};
		void*				m_MappedPtr{};
		uint32_t			m_Size{};
		EBufferType			m_Type;
	};

} // namespace DM::RHI
