#include<Core/RHI/Backend/Vulkan/Buffer/VulkanIndexBuffer.h>
#include<Core/RHI/Backend/Vulkan/VulkanDevice.h>
namespace DM::RHI
{
	VulkanIndexBuffer::VulkanIndexBuffer(VulkanDevice* device, const RHIIndexBufferDesc& desc):VulkanBuffer(device)
	{

		m_Size = desc.Indices.size() * sizeof(desc.Indices[0]);

		VkBuffer temporaryvkBuffer{};
		VkDeviceMemory temporaryvkMemory{};
		void* temporaryMapedPtr = nullptr;

		//创建CPU可写的用于作为数据传输源的中间缓存
		device->CreatevkBuffer(m_Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, temporaryvkBuffer, temporaryvkMemory);
		//映射到对应内存局域(获取指向刚分配的中间内存的指针)
		vkMapMemory(m_Device->GetvkDevice(), temporaryvkMemory, 0, m_Size, 0, &temporaryMapedPtr);
		memcpy(temporaryMapedPtr, desc.Indices.data(), (size_t)m_Size);//写入数据
		vkUnmapMemory(m_Device->GetvkDevice(), temporaryvkMemory);//解除映射

		device->CreatevkBuffer(m_Size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vkBuffer,m_vkMemory);
	

		CopyBuffer(temporaryvkBuffer, m_vkBuffer);
		vkDestroyBuffer(m_Device->GetvkDevice(), temporaryvkBuffer, nullptr);
		vkFreeMemory(m_Device->GetvkDevice(), temporaryvkMemory, nullptr);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
	}
}