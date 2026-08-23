#include<Core/RHI/Backend/Vulkan/VulkanShaderProgram.h>
#include<Core/RHI/Backend/Vulkan/VulkanShader.h>
#include<Core/RHI/Backend/Vulkan/VulkanDevice.h>
#include<Core/RHI/Backend/Vulkan/VulkanTexture.h>
#include<Core/RHI/Backend/Vulkan/VulkanDescriptorSet.h>
#include<Core/RHI/Backend/Vulkan/VulkanDescriptorSetGroup.h>
#include<Core/RHI/Backend/Vulkan/Buffer/VulkanUniformBuffer.h>
#include<ranges>
namespace DM::RHI
{
	VulkanShaderProgram::VulkanShaderProgram(VulkanDevice* device, const RHIShaderProgramDesc& desc):m_Device(device)
	{
		CreateShaderStageCreateInfo(desc);
		CreateVertexInput(desc);
		CreateSetLayout(desc);
		CreatePipelineLayout(desc);

		std::ranges::for_each(desc.Shaders, [this](RHIShader*shader) 
		{
			const auto& ref = shader->GetReflectionInfo();

			auto& ubos = m_Reflection.uniformBuffers;
			ubos.insert(ubos.end(), ref.uniformBuffers.begin(), ref.uniformBuffers.end());

			auto& texs = m_Reflection.textures;
			texs.insert(texs.end(), ref.textures.begin(), ref.textures.end());
		});
	}

	VulkanShaderProgram::~VulkanShaderProgram()
	{
		for(const auto&layout: m_vkSetLayouts)vkDestroyDescriptorSetLayout(m_Device->GetvkDevice(), layout.second, nullptr);
		vkDestroyPipelineLayout(m_Device->GetvkDevice(), m_vkPipelineLayout, nullptr);
	}

	void VulkanShaderProgram::CreateShaderStageCreateInfo(const RHIShaderProgramDesc& desc)
	{
		m_vkPipelineShaderStages.reserve(desc.Shaders.size());
		for (RHIShader* shader : desc.Shaders)
		{
			VulkanShader* vulkanShader = static_cast<VulkanShader*>(shader);
			
			m_vkPipelineShaderStages.emplace_back(vulkanShader->GenvkShaderStageCreateInfo());
		}
	}

	void VulkanShaderProgram::CreateVertexInput(const RHIShaderProgramDesc& desc)
	{
		auto vertexIt = std::ranges::find_if(desc.Shaders,
			[](RHIShader* shader) {
				return shader->GetStage() == EShaderStage::Vertex;
			});
		VulkanShader* vertexShader = (vertexIt != desc.Shaders.end()) ? static_cast<VulkanShader*>(*vertexIt): nullptr;

		if (!vertexShader)return;
		
		m_vkVertexInput = vertexShader->GenvkPipelineVertexInputStateCreateInfo();
	}

	void VulkanShaderProgram::CreateSetLayout(const RHIShaderProgramDesc& desc)
	{
		std::unordered_map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>>set_bindings;

		for (const RHIShader* shader : desc.Shaders)
		{
			const VulkanShader* vulkanShader = static_cast<const VulkanShader*>(shader);
			auto shaderBindings = vulkanShader->GenvkDescriptorSetLayoutBindings();
			for (const auto& it : shaderBindings)
			{
				auto& bindings = set_bindings[it.first];

				bindings.insert(bindings.end(), it.second.begin(), it.second.end());
			}
		}

		int i = 0;
		for (const auto& [set, bindings] : set_bindings)
		{
			VkDescriptorSetLayoutCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			info.bindingCount = bindings.size();
			info.pBindings = bindings.data();
			vkCreateDescriptorSetLayout(m_Device->GetvkDevice(), &info, nullptr, &m_vkSetLayouts[set]);
		}
	}


	void VulkanShaderProgram::CreatePipelineLayout(const RHIShaderProgramDesc& desc)
	{
		std::vector<VkPushConstantRange>ranges{};

		for (const RHIShader* shader : desc.Shaders)
		{
			const VulkanShader* vulkanShader = static_cast<const VulkanShader*>(shader);
			auto shaderRanges = vulkanShader->GenvkPushConstantRanges();
			ranges.insert(ranges.end(), shaderRanges.begin(), shaderRanges.end());
		}
		std::vector<VkDescriptorSetLayout>setLayouts;

		setLayouts.reserve(m_vkSetLayouts.size());

		std::ranges::for_each(m_vkSetLayouts, [&setLayouts](const auto& pair) {setLayouts.emplace_back(pair.second);});

		VkPipelineLayoutCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		info.setLayoutCount = setLayouts.size();
		info.pSetLayouts = setLayouts.data();
		info.pushConstantRangeCount = ranges.size();
		info.pPushConstantRanges = ranges.data();

		VK_CHECK(vkCreatePipelineLayout(m_Device->GetvkDevice(), &info, nullptr, &m_vkPipelineLayout));
	}

	const ShaderReflection::UniformBuffer* VulkanShaderProgram::FindUboInfo(const std::string& name)const
	{
		for (const auto& in : m_Reflection.uniformBuffers)
		{
			if (in.name == name)return &in;
		}
		return nullptr;
	}

	const ShaderReflection::TextureBinding* VulkanShaderProgram::FindTexInfo(const std::string& name)const
	{
		for (const auto& in : m_Reflection.textures)
		{
			if (in.name == name)return &in;
		}
		return nullptr;
	}

	RHIDescriptorSetGroup* VulkanShaderProgram::GenDescriptorSetGroup() const
	{
		RHIDescriptorSetGroup* group{};
		std::vector<VulkanDescriptorSet*>sets;
		sets.reserve(m_vkSetLayouts.size());

		std::unordered_map<uint32_t, std::vector<Descriptor>>set_Descriptors{};

		for (const auto& in : m_Reflection.uniformBuffers)	set_Descriptors[in.set].push_back(GenDescriptor(in));
		for (const auto& in : m_Reflection.textures)		set_Descriptors[in.set].emplace_back(GenDescriptor(in));

		for (const auto& [set, vkSetLayout] : m_vkSetLayouts)
		{
			RHIDescriptorSetDesc setDesc{};
			setDesc.Descriptors = set_Descriptors[set];

			VulkanDescriptorSet* vulkanSet = new VulkanDescriptorSet(m_Device, setDesc, &vkSetLayout);
			sets.emplace_back(vulkanSet);
		}

		group = new VulkanDescriptorSetGroup(m_Device,sets);

		return group;
	}


	std::vector<RHIDescriptorSet*> VulkanShaderProgram::GenDescriptorSets() const
	{
		std::vector<RHIDescriptorSet*>result;
		result.reserve(m_vkSetLayouts.size());

		std::unordered_map<uint32_t, std::vector<Descriptor>>set_Descriptors{};

		for (const auto& in : m_Reflection.uniformBuffers)	set_Descriptors[in.set].emplace_back(GenDescriptor(in));
		for (const auto& in : m_Reflection.textures)		set_Descriptors[in.set].emplace_back(GenDescriptor(in));

		for (const auto& [set, vkSetLayout] : m_vkSetLayouts)
		{
			RHIDescriptorSetDesc setDesc{};
			setDesc.Descriptors = set_Descriptors[set];

			VulkanDescriptorSet* vulkanSet = new VulkanDescriptorSet(m_Device, setDesc, &vkSetLayout);
			result.emplace_back(vulkanSet);
		}

		return result;
	}


	RHIDescriptorSet* VulkanShaderProgram::GenDescriptorSet(uint32_t set) const
	{
		RHIDescriptorSet* result{};

		std::vector<Descriptor> descriptors{};

		for (const auto& in : m_Reflection.uniformBuffers)
		{
			if (in.set != set)continue;
			descriptors.emplace_back(GenDescriptor(in));
		}

		for (const auto& in : m_Reflection.textures)
		{
			if (in.set != set)continue;
			descriptors.emplace_back(GenDescriptor(in));
		}

		if (auto it = m_vkSetLayouts.find(set); it != m_vkSetLayouts.end())
		{
			RHIDescriptorSetDesc setDesc{};
			setDesc.Descriptors = descriptors;

			result = new VulkanDescriptorSet(m_Device, setDesc, &it->second);
		}

		return result;
	}

	Descriptor VulkanShaderProgram::GenDescriptor(const ShaderReflection::UniformBuffer& info)const
	{
		Descriptor desc{};
		desc.arraySize	= info.arraySize;
		desc.m_binding	= info.binding;
		desc.m_Name		= info.name;
		desc.m_Type		= info.type;
		desc.m_Set		= info.set;
		return desc;
	}

	Descriptor VulkanShaderProgram::GenDescriptor(const ShaderReflection::TextureBinding& info)const
	{
		Descriptor desc{};
		desc.arraySize	= info.arraySize;
		desc.m_binding	= info.binding;
		desc.m_Name		= info.name;
		desc.m_Type		= info.type;
		desc.m_Set		= info.set;
		return desc;
	}

}