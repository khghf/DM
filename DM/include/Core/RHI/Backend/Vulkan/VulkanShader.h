#pragma once
#include"Core/RHI/RHIShader.h"
#include<vulkan/vulkan.h>
#include<string>
#include<unordered_map>
namespace DM::RHI
{
	class VulkanDevice;
	class VulkanShader : public RHIShader
	{
	public:
		VulkanShader(VulkanDevice* device, const RHIShaderDesc& desc);
		~VulkanShader() override;
	public:
		VkShaderModule			GetModule()     const { return m_Module; }
		VkShaderStageFlagBits	GetVkStage()  const { return m_vkStage; }
		const std::string&		GetEntryPoint() const { return m_EntryPoint; }


		VkPipelineShaderStageCreateInfo GenvkShaderStageCreateInfo()const;
		VkPipelineVertexInputStateCreateInfo GenvkPipelineVertexInputStateCreateInfo()const;
		std::unordered_map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>>GenvkDescriptorSetLayoutBindings()const;
		std::vector<VkPushConstantRange>GenvkPushConstantRanges()const;
	private:
		VulkanDevice*         m_Device;     
		VkShaderModule        m_Module;    
		VkShaderStageFlagBits m_vkStage;   
		std::string           m_EntryPoint;
	};
} // namespace DM::RHI
