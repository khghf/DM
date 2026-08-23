#include<Core/RHI/Backend/Vulkan/VulkanDescriptorSetGroup.h>
#include<Core/RHI/Backend/Vulkan/VulkanDescriptorSet.h>
#include<Core/RHI/Backend/Vulkan/Buffer/VulkanUniformBuffer.h>
#include<Core/RHI/Backend/Vulkan/VulkanTexture.h>
#include<Core/RHI/Backend/Vulkan/VulkanDevice.h>
#include<Core/RHI/Backend/Vulkan/VulkanTypeMapping.h>
namespace DM::RHI
{
	VulkanDescriptorSetGroup::VulkanDescriptorSetGroup(VulkanDevice* device,std::vector<VulkanDescriptorSet*> sets):m_Device(device)
	{
		m_Sets = sets;
	}

	VulkanDescriptorSetGroup::~VulkanDescriptorSetGroup()
	{

	}

	void VulkanDescriptorSetGroup::BindUBO(RHIBuffer* ubo, const std::string& name) const
	{
		for (const auto& descSet : m_Sets)
		{
			for (const auto& in : descSet->GetDescriptors())
			{
				if (in.m_Name == name)
				{
					descSet->BindUBO(ubo, name);
					return;
				}
			}
		}
		DM_CORE_ASSERT(false, "{}", "Bind UBO failed. Please check whether the passed name is correct.");
	}

	void VulkanDescriptorSetGroup::BindTex(RHITexture* tex, const std::string& name) const
	{
		for (const auto& descSet : m_Sets)
		{
			for (const auto& in : descSet->GetDescriptors())
			{
				if (in.m_Name == name)
				{
					descSet->BindTex(tex, name);
					return;
				}
			}
		}
		DM_CORE_ASSERT(false, "{}", "Bind TEX failed. Please check whether the passed name is correct.");

	}

	std::vector<VkDescriptorSet> VulkanDescriptorSetGroup::GetvkDescriptorSets() const
	{
		std::vector<VkDescriptorSet>res;

		res.reserve(m_Sets.size());

		for (const auto& in : m_Sets)
		{
			res.emplace_back(in->GetvkDescriptorSet());
		}

		return res;
	}
}