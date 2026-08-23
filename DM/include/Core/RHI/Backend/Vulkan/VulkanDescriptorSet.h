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

		uint32_t GetSetId()const { return m_Set; }
		VkDescriptorSet GetvkDescriptorSet()const { return m_vkDescriptorSet; }


		virtual void BindUBO(RHIBuffer* ubo, const std::string& name)const override;
		virtual void BindTex(RHITexture* tex, const std::string& name)const override;
	private:
		VulkanDevice*	m_Device;
		VkDescriptorSet m_vkDescriptorSet;
		uint32_t		m_Set;

	};
}