#include<Core/RHI/Backend/Vulkan/VulkanShader.h>
#include<Core/RHI/Backend/Vulkan/VulkanDevice.h>
#include<Core/RHI/Backend/Vulkan/VulkanTypeMapping.h>
#include<Core/RHI/ShaderCompiler.h>
namespace DM::RHI
{
	VulkanShader::VulkanShader(VulkanDevice* device, const RHIShaderDesc& desc)
		: m_Device(device), m_vkStage(ToVkShaderStage(desc.Stage))
	{
		m_ReflectionInfo = desc.Reflection;
		m_Stage = desc.Stage;
		m_EntryPoint = (desc.EntryPoint && desc.EntryPoint[0] != '\0') ? desc.EntryPoint : "main";

		VkShaderModuleCreateInfo ci{};
		ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		ci.codeSize = desc.CodeBytes;
		ci.pCode = reinterpret_cast<const uint32_t*>(desc.Code); // SPIR-V 以 4 字节为单位的单元
		VK_CHECK(vkCreateShaderModule(m_Device->GetvkDevice(), &ci, nullptr, &m_Module));
	}

	VulkanShader::~VulkanShader()
	{
		vkDestroyShaderModule(m_Device->GetvkDevice(), m_Module, nullptr);
	}

	VkPipelineShaderStageCreateInfo VulkanShader::GenvkShaderStageCreateInfo() const
	{
		VkPipelineShaderStageCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		info.stage = ToVkShaderStage(m_Stage);
		info.module = GetModule();
		info.pName = GetEntryPoint().c_str();

		return info;
	}

	VkPipelineVertexInputStateCreateInfo VulkanShader::GenvkPipelineVertexInputStateCreateInfo()const
	{
		VkPipelineVertexInputStateCreateInfo info{};
		if (m_Stage != EShaderStage::Vertex) return info;

		static std::vector<VkVertexInputAttributeDescription> attributeDescs{};
		static std::vector<VkVertexInputBindingDescription> bindingDescs{};
		attributeDescs.clear();
		bindingDescs.clear();

		std::unordered_map<uint32_t, uint32_t> bindingStride;

		// 收集属性
		for (const auto& in : m_ReflectionInfo.vertexInputs)
		{
			VkVertexInputAttributeDescription attributeDesc{};
			attributeDesc.binding = 0;
			attributeDesc.format = ToVkFormat(in.format);
			attributeDesc.location = in.location;
			attributeDesc.offset = in.offset;

			if (in.size + in.offset > bindingStride[attributeDesc.binding])bindingStride[attributeDesc.binding] = in.size + in.offset;
			attributeDescs.push_back(attributeDesc);
		}

		//  为每个binding 创建描述
		for (const auto& [binding, stride] : bindingStride)
		{
			VkVertexInputBindingDescription bindingDesc{};
			bindingDesc.binding = binding;
			bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			bindingDesc.stride = stride;
			bindingDescs.push_back(bindingDesc); 
		}

		info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		info.pVertexAttributeDescriptions = attributeDescs.data();
		info.vertexAttributeDescriptionCount = attributeDescs.size();
		info.pVertexBindingDescriptions = bindingDescs.data();
		info.vertexBindingDescriptionCount = bindingDescs.size();

		return info;
	}



	std::unordered_map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> VulkanShader::GenvkDescriptorSetLayoutBindings() const
	{
		std::unordered_map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> result;

		for (const auto& in : m_ReflectionInfo.uniformBuffers)
		{
			VkDescriptorSetLayoutBinding binding{};
			binding.binding = in.binding;
			binding.descriptorCount = in.arraySize;
			binding.pImmutableSamplers = nullptr;
			binding.stageFlags = ToVkShaderStage(m_Stage);
			binding.descriptorType = ToVkDescriptorType(in.type);
			result[in.set].emplace_back(binding);
		}

		for (const auto& in : m_ReflectionInfo.textures)
		{
			VkDescriptorSetLayoutBinding binding{};
			binding.binding = in.binding;
			binding.descriptorCount = in.arraySize;
			binding.descriptorType = ToVkDescriptorType(in.type);
			binding.pImmutableSamplers = nullptr;
			binding.stageFlags = ToVkShaderStage(m_Stage);
			result[in.set].emplace_back(binding);
		}
		return result;
	}
	std::vector<VkPushConstantRange> VulkanShader::GenvkPushConstantRanges() const
	{
		std::vector<VkPushConstantRange>result{};
		result.reserve(m_ReflectionInfo.pushConstants.size());
		for (const auto& in : m_ReflectionInfo.pushConstants)
		{
			VkPushConstantRange pc{};
			pc.offset = in.offset;
			pc.size = in.size;
			pc.stageFlags = ToVkShaderStage(in.stage);
			result.emplace_back(pc);
		}
		return result;
	}
} // namespace DM::RHI
