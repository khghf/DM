#include<Core/RHI/Backend/Vulkan/Buffer/VulkanUniformBuffer.h>
#include<Core/RHI/Backend/Vulkan/VulkanDevice.h>
#include<Core/RHI/Backend/Vulkan/VulkanTypeMapping.h>

namespace DM::RHI
{
	VulkanUniformBuffer::VulkanUniformBuffer(VulkanDevice* device, const RHIUniformBufferDesc& desc) :VulkanBuffer(device)
	{
		m_bDynamic = desc.m_bDynamic;
		m_Size = desc.m_SizeBytes;
		
	}
	VulkanUniformBuffer::~VulkanUniformBuffer()
	{

	}
	void VulkanUniformBuffer::Update(const void* data, size_t size)
	{
		memcpy(GetMappedPtr(), data, size);
	}
}