#pragma once
#include"../../RHIDescriptorSetGroup.h"
#include<unordered_map>
#include<vulkan/vulkan.h>
namespace DM::RHI
{
	class VulkanDescriptorSet;
	class VulkanDevice;
	//class VulkanDescriptorSet;
	class VulkanDescriptorSetGroup:public RHIDescriptorSetGroup
	{
	public:
		VulkanDescriptorSetGroup(VulkanDevice*device,std::vector<VulkanDescriptorSet*>sets);

		~VulkanDescriptorSetGroup();

		virtual void BindUBO(RHIBuffer* ubo, const std::string& name)const override;
		virtual void BindTex(RHITexture* tex, const std::string& name)const override;


		std::vector<VkDescriptorSet> GetvkDescriptorSets()const;


	private:
		VulkanDevice* m_Device;

		std::vector<VulkanDescriptorSet*>m_Sets;
	};
}