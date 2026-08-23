#include<Core/RHI/Backend/Vulkan/VulkanDescriptorSet.h>
#include<Core/RHI/Backend/Vulkan/VulkanDevice.h>
#include<Core/RHI/Backend/Vulkan/VulkanTexture.h>
#include<Core/RHI/Backend/Vulkan/Buffer/VulkanUniformBuffer.h>
#include<Core/RHI/Backend/Vulkan/VulkanTypeMapping.h>
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

	void VulkanDescriptorSet::BindUBO(RHIBuffer* ubo, const std::string& name) const
	{
		for (const auto& descriptor : m_Descriptors)
		{
			if (name == descriptor.m_Name)
			{
				VulkanUniformBuffer* vulkanResource = static_cast<VulkanUniformBuffer*>(ubo);
				VkDescriptorBufferInfo info{};
				info.buffer = vulkanResource->GetvkBuffer();
				info.offset = 0;
				info.range = vulkanResource->GetSize();

				VkWriteDescriptorSet write{};
				write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				write.descriptorType = ToVkDescriptorType(descriptor.m_Type);
				write.dstArrayElement = 0;//开始写入的数组索引
				write.descriptorCount = 1;//总的写入数量
				write.dstBinding = descriptor.m_binding;
				write.dstSet = GetvkDescriptorSet();
				write.pBufferInfo = &info;

				vkUpdateDescriptorSets(m_Device->GetvkDevice(), 1, &write, 0, nullptr);
				return;
			}
		}
		DM_CORE_ASSERT(false, "{}", "Bind UBO failed. Please check whether the passed name is correct.");
	}

	void VulkanDescriptorSet::BindTex(RHITexture* tex, const std::string& name) const
	{
		for (const auto& descriptor : m_Descriptors)
		{
			if (name == descriptor.m_Name)
			{
				VulkanTexture* vulkanResource = static_cast<VulkanTexture*>(tex);
				VkDescriptorImageInfo info{};
				info.imageLayout = vulkanResource->GetvkImageLayout();
				info.imageView = vulkanResource->GetvkImageView();
				info.sampler = vulkanResource->GetvkSampler();

				VkWriteDescriptorSet write{};
				write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				write.descriptorType = ToVkDescriptorType(descriptor.m_Type);
				write.dstArrayElement = 0;//开始写入的数组索引
				write.descriptorCount = 1;//总的写入数量
				write.dstBinding = descriptor.m_binding;
				write.dstSet = GetvkDescriptorSet();
				write.pImageInfo = &info;

				vkUpdateDescriptorSets(m_Device->GetvkDevice(), 1, &write, 0, nullptr);
				return;
			}
		}
		DM_CORE_ASSERT(false, "{}", "Bind TEX failed. Please check whether the passed name is correct.");
	}
}