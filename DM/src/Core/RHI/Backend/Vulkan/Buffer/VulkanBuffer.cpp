#include"Core/RHI/Backend/Vulkan/Buffer/VulkanBuffer.h"
#include"Core/RHI/Backend/Vulkan/VulkanDevice.h"
#include<Core/RHI/Backend/Vulkan/VulkanTypeMapping.h>

namespace DM::RHI
{
	VulkanBuffer::VulkanBuffer(VulkanDevice* device)
		: m_Device(device)
	{
		
	}

	VulkanBuffer::VulkanBuffer(VulkanDevice* device, const RHIBufferDesc& desc) : m_Device(device), m_Type(desc.Type),m_Size(desc.SizeBytes)
	{
		switch (m_Type)
		{
		case DM::RHI::EBufferType::VertexBuffer:		CreateVertexBuffer(desc);
			break;
		case DM::RHI::EBufferType::IndexBuffer:			CreateIndexBuffer(desc);
			break;

		case DM::RHI::EBufferType::UniformBuffer:
		case DM::RHI::EBufferType::DynamicUniformBuffer:CreateUniformBuffer(desc);
			break;
		case DM::RHI::EBufferType::StorageBuffer:
		case DM::RHI::EBufferType::DynamicStorageBuffer:CreateStorageBuffer(desc);
			break;
		case DM::RHI::EBufferType::IndirectBuffer:
			break;
		case DM::RHI::EBufferType::StagingBuffer:
			break;
		default:
			break;
		}
	}

	VulkanBuffer::~VulkanBuffer()
	{
		if ( m_MappedPtr) vkUnmapMemory(m_Device->GetvkDevice(),  m_vkMemory); 
		vkDestroyBuffer(m_Device->GetvkDevice(),  m_vkBuffer, nullptr);
		vkFreeMemory(m_Device->GetvkDevice(),  m_vkMemory, nullptr);
	}

	void VulkanBuffer::Update(const void* data, size_t size)
	{
		switch (m_Type)
		{
		case DM::RHI::EBufferType::VertexBuffer:		UpdateVertexBuffer(data, size);
			break;
		case DM::RHI::EBufferType::IndexBuffer:			UpdateIndexBuffer(data, size);
			break;
		case DM::RHI::EBufferType::UniformBuffer:
		case DM::RHI::EBufferType::DynamicUniformBuffer:UpdateUniformBuffer(data, size);
			break;
		case DM::RHI::EBufferType::StorageBuffer:
		case DM::RHI::EBufferType::DynamicStorageBuffer:UpdateStorageBuffer(data, size);
			break;
		case DM::RHI::EBufferType::IndirectBuffer:
			break;
		case DM::RHI::EBufferType::StagingBuffer:
			break;
		default:
			break;
		}
	}

	void* VulkanBuffer::Map()
	{
		//将内存实际的物理地址映射为CPU的虚拟地址以便CPU能够访问
		VK_CHECK(vkMapMemory(m_Device->GetvkDevice(),  m_vkMemory, 0, m_Size, 0, & m_MappedPtr));
		return GetMappedPtr();
	}

	void VulkanBuffer::Unmap()
	{
		vkUnmapMemory(m_Device->GetvkDevice(),  m_vkMemory);
		m_MappedPtr = nullptr;
	}

	void VulkanBuffer::CopyBuffer( VkBuffer src, VkBuffer dest)
	{
		VkCommandBuffer commandBuffer = m_Device->BeginSingleTimeCommands();

		VkBufferCopy copyRegion{};
		copyRegion.size = m_Size;
		vkCmdCopyBuffer(commandBuffer, src, dest, 1, &copyRegion);

		m_Device->EndSingleTimeCommands();
	}



	void VulkanBuffer::CreateVertexBuffer(const RHIBufferDesc& desc)
	{
		m_Device->CreatevkBuffer(m_Size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vkBuffer, m_vkMemory);
	}

	void VulkanBuffer::UpdateVertexBuffer(const void* data, size_t size)
	{
		VkBuffer temporaryvkBuffer{};
		VkDeviceMemory temporaryvkMemory{};
		void* temporaryMapedPtr = nullptr;
		//创建CPU可写的用于作为数据传输源的中间缓存
		m_Device->CreatevkBuffer(m_Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, temporaryvkBuffer, temporaryvkMemory);
		//映射到对应内存局域(获取指向刚分配的中间内存的指针)
		vkMapMemory(m_Device->GetvkDevice(), temporaryvkMemory, 0, m_Size, 0, &temporaryMapedPtr);
		memcpy(temporaryMapedPtr, data, size);//写入数据
		vkUnmapMemory(m_Device->GetvkDevice(), temporaryvkMemory);//解除映射
		temporaryMapedPtr = nullptr;

		//将数据复制到GPU本地内存
		CopyBuffer(temporaryvkBuffer, m_vkBuffer);

		vkDestroyBuffer(m_Device->GetvkDevice(), temporaryvkBuffer, nullptr);
		vkFreeMemory(m_Device->GetvkDevice(), temporaryvkMemory, nullptr);
	}

	void VulkanBuffer::CreateIndexBuffer(const RHIBufferDesc& desc)
	{
		m_Device->CreatevkBuffer(m_Size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vkBuffer, m_vkMemory);
	}

	void VulkanBuffer::UpdateIndexBuffer(const void* data, size_t size)
	{
		VkBuffer temporaryvkBuffer{};
		VkDeviceMemory temporaryvkMemory{};
		void* temporaryMapedPtr = nullptr;

		//创建CPU可写的用于作为数据传输源的中间缓存
		m_Device->CreatevkBuffer(m_Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, temporaryvkBuffer, temporaryvkMemory);
		//映射到对应内存局域(获取指向刚分配的中间内存的指针)
		vkMapMemory(m_Device->GetvkDevice(), temporaryvkMemory, 0, m_Size, 0, &temporaryMapedPtr);
		memcpy(temporaryMapedPtr, data, size);//写入数据
		vkUnmapMemory(m_Device->GetvkDevice(), temporaryvkMemory);//解除映射

		CopyBuffer(temporaryvkBuffer, m_vkBuffer);
		vkDestroyBuffer(m_Device->GetvkDevice(), temporaryvkBuffer, nullptr);
		vkFreeMemory(m_Device->GetvkDevice(), temporaryvkMemory, nullptr);
	}

	void VulkanBuffer::CreateUniformBuffer(const RHIBufferDesc& desc)
	{
		m_Device->CreatevkBuffer(m_Size, ToVkBufferUsage(desc.Type), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_vkBuffer, m_vkMemory);
		Map();
	}

	void VulkanBuffer::UpdateUniformBuffer(const void* data, size_t size)
	{
		memcpy(GetMappedPtr(), data, size);
	}

	void VulkanBuffer::CreateStorageBuffer(const RHIBufferDesc& desc)
	{
		CreateUniformBuffer(desc);
	}

	void VulkanBuffer::UpdateStorageBuffer(const void* data, size_t size)
	{
		UpdateUniformBuffer(data, size);
	}



} // namespace DM::RHI
