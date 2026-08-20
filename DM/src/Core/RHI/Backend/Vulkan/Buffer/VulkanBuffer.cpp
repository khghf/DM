#include"Core/RHI/Backend/Vulkan/Buffer/VulkanBuffer.h"
#include"Core/RHI/Backend/Vulkan/VulkanDevice.h"

namespace DM::RHI
{
	VulkanBuffer::VulkanBuffer(VulkanDevice* device)
		: m_Device(device)
	{
		
	}

	VulkanBuffer::~VulkanBuffer()
	{
		if ( m_MappedPtr) vkUnmapMemory(m_Device->GetvkDevice(),  m_vkMemory); 
		vkDestroyBuffer(m_Device->GetvkDevice(),  m_vkBuffer, nullptr);
		vkFreeMemory(m_Device->GetvkDevice(),  m_vkMemory, nullptr);
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
} // namespace DM::RHI
