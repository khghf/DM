#pragma once
#include"../../RHIDescriptorSet.h"
#include<vulkan/vulkan.h>
namespace DM::RHI
{
	class VulkanDevice;
	class VulkanDescriptorSet:public RHIDescriptorSet
	{
	public:
		VulkanDescriptorSet(VulkanDevice* device,const RHIDescriptorSetDesc&desc,const VkDescriptorSetLayout* setLayout);
		~VulkanDescriptorSet();

		uint32_t GetSetNum()const { return m_Set; }
		VkDescriptorSet GetvkDescriptorSet()const { return m_vkDescriptorSet; }
	private:
		VulkanDevice* m_Device;
		VkDescriptorSet m_vkDescriptorSet;
		uint32_t m_Set;

	};
}