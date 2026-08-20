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

		VulkanUniformBuffer* vulkanResource = static_cast<VulkanUniformBuffer*>(ubo);

		for (const auto& descSet : m_Sets)
		{
			for (const auto& in : descSet->GetDescriptors())
			{
				if (in.m_Name == name)
				{
					VulkanDescriptorSet* vulkanSet = static_cast<VulkanDescriptorSet*>(descSet);

					VkDescriptorBufferInfo info{};
					info.buffer = vulkanResource->GetvkBuffer();
					info.offset = 0;
					info.range = vulkanResource->GetSize();

					VkWriteDescriptorSet write{};
					write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					write.descriptorType = ToVkDescriptorType(in.m_Type);
					write.dstArrayElement = 0;//开始写入的数组索引
					write.descriptorCount = 1;//总的写入数量
					write.dstBinding = in.m_binding;
					write.dstSet = vulkanSet->GetvkDescriptorSet();
					write.pBufferInfo = &info;

					vkUpdateDescriptorSets(m_Device->GetvkDevice(), 1, &write, 0, nullptr);

					return;
				}
			}
		}

		DM_CORE_ASSERT(false, "{}", "Bind UBO failed. Please check whether the passed name is correct.");
	}

	void VulkanDescriptorSetGroup::BindTex(RHITexture* tex, const std::string& name) const
	{
		VulkanTexture* vulkanResource = static_cast<VulkanTexture*>(tex);
		for (const auto& descSet : m_Sets)
		{
			for (const auto& in : descSet->GetDescriptors())
			{
				if (in.m_Name == name)
				{
					VulkanDescriptorSet* vulkanSet = static_cast<VulkanDescriptorSet*>(descSet);

					VkDescriptorImageInfo info{};
					info.imageLayout = vulkanResource->GetvkImageLayout();
					info.imageView = vulkanResource->GetvkImageView();
					info.sampler = vulkanResource->GetvkSampler();

					VkWriteDescriptorSet write{};
					write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					write.descriptorType = ToVkDescriptorType(in.m_Type);
					write.dstArrayElement = 0;//开始写入的数组索引
					write.descriptorCount = 1;//总的写入数量
					write.dstBinding = in.m_binding;
					write.dstSet = vulkanSet->GetvkDescriptorSet();
					write.pImageInfo = &info;

					vkUpdateDescriptorSets(m_Device->GetvkDevice(), 1, &write, 0, nullptr);
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