#include<Core/RHI/Backend/Vulkan/Buffer/VulkanUniformBuffer.h>
#include<Core/RHI/Backend/Vulkan/VulkanDevice.h>
namespace DM::RHI
{
	VulkanUniformBuffer::VulkanUniformBuffer(VulkanDevice* device, const RHIUniformBufferDesc& desc) :VulkanBuffer(device)
	{
		m_bDynamic = desc.m_bDynamic;
		m_Size = desc.m_SizeBytes;
		m_Device->CreatevkBuffer(m_Size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,m_vkBuffer, m_vkMemory);
		Map();
	}
	VulkanUniformBuffer::~VulkanUniformBuffer()
	{

	}
	void VulkanUniformBuffer::UpdateData(const void* data, size_t size)
	{
		memcpy(GetMappedPtr(), data, size);
	}
}