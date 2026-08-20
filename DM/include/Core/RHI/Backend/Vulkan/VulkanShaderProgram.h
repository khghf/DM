#pragma once
#include"../../RHIShaderProgram.h"
#include<vulkan/vulkan.h>
namespace DM::RHI
{
	class VulkanDevice;
	class VulkanShaderProgram:public RHIShaderProgram
	{
		friend class VulkanPipeline;
		friend class VulkanCommandList;
	public:
		VulkanShaderProgram(VulkanDevice*device,const RHIShaderProgramDesc&desc);
		~VulkanShaderProgram();

	protected:
		void CreateShaderStageCreateInfo(const RHIShaderProgramDesc& desc);
		void CreateVertexInput(const RHIShaderProgramDesc& desc);
		void CreateSetLayout(const RHIShaderProgramDesc& desc);
		void CreatePipelineLayout(const RHIShaderProgramDesc& desc);

		const ShaderReflection::UniformBuffer*	FindUboInfo(const std::string& name)const;
		const ShaderReflection::TextureBinding*	FindTexInfo(const std::string& name)const;

	public:

		virtual std::vector<RHIDescriptorSet*>	GenDescriptorSets()const override;
		virtual RHIDescriptorSetGroup*			GenDescriptorSetGroup()const override;

	private:
		VulkanDevice* m_Device;

		VkPipelineVertexInputStateCreateInfo				m_vkVertexInput;
		std::unordered_map<uint32_t, VkDescriptorSetLayout>	m_vkSetLayouts;
		VkPipelineLayout									m_vkPipelineLayout;
		std::vector<VkPipelineShaderStageCreateInfo>		m_vkPipelineShaderStages;
		ShaderReflection									m_Reflection;

	};
}