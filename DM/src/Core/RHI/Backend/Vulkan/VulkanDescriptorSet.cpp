#include<Core/RHI/Backend/Vulkan/VulkanDescriptorSet.h>
#include<Core/RHI/Backend/Vulkan/VulkanDevice.h>
namespace DM::RHI
{

	VulkanDescriptorSet::VulkanDescriptorSet(VulkanDevice* device, const RHIDescriptorSetDesc& desc, const VkDescriptorSetLayout* setLayout) :m_Device(device)
	{
		m_vkDescriptorSet = m_Device->AllocateDescriptorSet(setLayout);
		m_Descriptors = desc.Descriptors;

		m_Set = m_Descriptors.size() > 0 ? m_Descriptors[0].m_Set : 0;
	}

	VulkanDescriptorSet::~VulkanDescriptorSet()
	{
		m_Device->FreeDescriptorSet(&m_vkDescriptorSet);
	}
}