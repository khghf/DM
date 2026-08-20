#include <Core/RHI/ShaderCompiler.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include<Core/Reflection/Mirror/include/mirror.h>




namespace DM::RHI
{
	REGISTER_TYPE(ShaderReflection);
	REGISTER_MEMBER(ShaderReflection, vertexInputs);
	REGISTER_MEMBER(ShaderReflection, uniformBuffers);
	REGISTER_MEMBER(ShaderReflection, textures);
	REGISTER_MEMBER(ShaderReflection, pushConstants);

	REGISTER_TYPE(ShaderReflection::VertexInput);
	REGISTER_MEMBER(ShaderReflection::VertexInput, location);
	REGISTER_MEMBER(ShaderReflection::VertexInput, format);
	REGISTER_MEMBER(ShaderReflection::VertexInput, offset);
	REGISTER_MEMBER(ShaderReflection::VertexInput, size);
	REGISTER_MEMBER(ShaderReflection::VertexInput, name);


	REGISTER_TYPE(ShaderReflection::UniformMember);
	REGISTER_MEMBER(ShaderReflection::UniformMember, name);
	REGISTER_MEMBER(ShaderReflection::UniformMember, offset);
	REGISTER_MEMBER(ShaderReflection::UniformMember, size);

	REGISTER_TYPE(ShaderReflection::UniformBuffer);
	REGISTER_MEMBER(ShaderReflection::UniformBuffer, set);
	REGISTER_MEMBER(ShaderReflection::UniformBuffer, binding);
	REGISTER_MEMBER(ShaderReflection::UniformBuffer, name);
	REGISTER_MEMBER(ShaderReflection::UniformBuffer, size);
	REGISTER_MEMBER(ShaderReflection::UniformBuffer, arraySize);
	REGISTER_MEMBER(ShaderReflection::UniformBuffer, type);
	REGISTER_MEMBER(ShaderReflection::UniformBuffer, members);


	REGISTER_TYPE(ShaderReflection::TextureBinding);
	REGISTER_MEMBER(ShaderReflection::TextureBinding, set);
	REGISTER_MEMBER(ShaderReflection::TextureBinding, binding);
	REGISTER_MEMBER(ShaderReflection::TextureBinding, name);
	REGISTER_MEMBER(ShaderReflection::TextureBinding, arraySize);
	REGISTER_MEMBER(ShaderReflection::TextureBinding, type);

	REGISTER_TYPE(ShaderReflection::PushConstant);
	REGISTER_MEMBER(ShaderReflection::PushConstant, stage);
	REGISTER_MEMBER(ShaderReflection::PushConstant, offset);
	REGISTER_MEMBER(ShaderReflection::PushConstant, size);
	REGISTER_MEMBER(ShaderReflection::PushConstant, members);

	ERHIFormat SPIRTypeToRHIFormat(const spirv_cross::SPIRType& type)
	{
		switch (type.basetype)
		{
		case spirv_cross::SPIRType::Float:
			switch (type.vecsize)
			{
			case 1: return ERHIFormat::Float;
			case 2: return ERHIFormat::Float2;
			case 3: return ERHIFormat::Float3;
			case 4: return ERHIFormat::Float4;
			}
			break;
		case spirv_cross::SPIRType::Int:
			switch (type.vecsize)
			{
			case 1: return ERHIFormat::Int;
			case 2: return ERHIFormat::Int2;
			case 3: return ERHIFormat::Int3;
			case 4: return ERHIFormat::Int4;
			}
			break;
		case spirv_cross::SPIRType::UInt:
			switch (type.vecsize)
			{
			case 1: return ERHIFormat::Uint;
			case 2: return ERHIFormat::Uint2;
			case 3: return ERHIFormat::Uint3;
			case 4: return ERHIFormat::Uint4;
			}
			break;
		}
		return ERHIFormat::Unknown;
	}


	uint32_t GetArraySize(const spirv_cross::SPIRType& type) {
		if (type.array.empty()) {
			return 1; 
		}

		uint32_t size = 1;
		for (auto dim : type.array) {
			size *= dim;
		}
		return size;
	}
	// ============================================================
	// IncludeHandler::GetInclude
	// 处理着色器中的 #include "xxx.glsl"
	// 按顺序搜索：
	//   1. 相对路径(相对于当前着色器文件)
	//   2. 用户添加的 include 搜索路径
	//   3. 直接使用请求的文件名
	// ============================================================
	shaderc_include_result* IncludeHandler::GetInclude(
		const char* requested_source,		// 被 include 的文件名，如 "common.glsl"
		shaderc_include_type type,          // 相对 include 还是系统 include
		const char* requesting_source,      // 发起 include 的文件
		size_t include_depth)               // include 嵌套深度
	{
		std::string fullPath;
		bool found = false;

		// ---- 第 1 步：相对路径 ----
		// 例如：shader.vert 中 #include "common.glsl"
		// -> 在 shader.vert 同目录下找 common.glsl
		if (type == shaderc_include_type_relative && requesting_source)
		{
			std::string dir = GetDirectory(requesting_source);
			fullPath = dir + "/" + requested_source;
			found = std::ifstream(fullPath).good();
		}

		// ---- 第 2 步：include 搜索路径 ----
		// 例如：AddIncludePath("Shaders/Common")
		// -> 在 Shaders/Common/ 下找 common.glsl
		if (!found)
		{
			for (const auto& path : m_IncludePaths)
			{
				fullPath = path + "/" + requested_source;
				if (std::ifstream(fullPath).good())
				{
					found = true;
					break;
				}
			}
		}

		// ---- 第 3 步：直接尝试 ----
		// 直接用请求的文件名
		if (!found)
		{
			fullPath = requested_source;
			found = std::ifstream(fullPath).good();
		}

		// 没找到文件
		if (!found)
		{
			return new shaderc_include_result{
				nullptr, 0, nullptr, 0, nullptr
			};
		}

		// 读取文件内容
		std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
		size_t size = file.tellg();
		file.seekg(0);

		char* content = new char[size];
		file.read(content, size);

		// 复制文件名
		char* name = new char[fullPath.size() + 1];
		strcpy(name, fullPath.c_str());

		// 返回结果(shaderc 会接管内存)
		return new shaderc_include_result{
			name, fullPath.size(),      // 文件名 + 长度
			content, size,              // 内容 + 长度
			nullptr                     // 用户数据(未使用)
		};
	}

	void IncludeHandler::ReleaseInclude(shaderc_include_result* data)
	{
		if (data)
		{
			delete[] data->source_name;  
			delete[] data->content;     
			delete data;                
		}
	}

	void IncludeHandler::AddIncludePath(const std::string& path)
	{
		m_IncludePaths.push_back(path);
	}

	std::string IncludeHandler::GetDirectory(const std::string& path)
	{
		size_t pos = path.find_last_of("/\\"); 
		return pos != std::string::npos ? path.substr(0, pos) : ".";
	}



	ShaderCompiler::ShaderCompiler()
		: m_IncludeHandler(new IncludeHandler())
	{
		m_Options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
		m_Options.SetTargetSpirv(shaderc_spirv_version_1_5);
		m_Options.SetOptimizationLevel(shaderc_optimization_level_performance);
		m_Options.SetSourceLanguage(shaderc_source_language_glsl);
		m_Options.SetGenerateDebugInfo();

		m_Options.SetIncluder(std::unique_ptr<IncludeHandler>(m_IncludeHandler));
	}

	void ShaderCompiler::AddIncludePath(const std::string& path)
	{
		m_IncludeHandler->AddIncludePath(path);
	}

	shaderc_shader_kind ShaderCompiler::GetShaderKind(EShaderStage stage)
	{
		switch (stage)
		{
		case EShaderStage::Vertex:              return shaderc_vertex_shader;
		case EShaderStage::Fragment:            return shaderc_fragment_shader;
		case EShaderStage::Compute:             return shaderc_compute_shader;
		case EShaderStage::TessellationControl: return shaderc_tess_control_shader;
		case EShaderStage::TessellationEvaluation: return shaderc_tess_evaluation_shader;
		case EShaderStage::Geometry:            return shaderc_geometry_shader;
		default: throw std::runtime_error("Unknown shader stage");
		}
	}

	CompiledShader ShaderCompiler::CompileGLSLFileToSPV(const std::string& filename, EShaderStage stage)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if (!file.is_open())
		{
			return { {}, "Failed to open file: " + filename, false };
		}

		size_t size = file.tellg();
		std::string source(size, ' ');
		file.seekg(0);
		file.read(source.data(), size);
		file.close();

		return CompileGLSLSourceToSPV(source, stage, filename);
	}

	CompiledShader ShaderCompiler::CompileGLSLSourceToSPV(
		const std::string& source,
		EShaderStage stage,
		const std::string& filename)
	{
		auto kind = GetShaderKind(stage);
		auto compiled = m_Compiler.CompileGlslToSpv(source, kind, filename.c_str(), m_Options);

		if (compiled.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			return { {}, compiled.GetErrorMessage(), false };
		}

		return {
			std::vector<uint32_t>(compiled.cbegin(), compiled.cend()),
			"",
			true
		};
	}


	ShaderReflection ShaderCompiler::Reflect(const std::vector<uint32_t>& spirv, EShaderStage stage)
	{
		ShaderReflection reflection;
		spirv_cross::CompilerReflection reflector(spirv);
		spirv_cross::ShaderResources resources = reflector.get_shader_resources();

		// ============================================================
		// 顶点输入属性（仅顶点着色器）
		// ============================================================
		if (stage == EShaderStage::Vertex)
		{
			uint32_t offset = 0;
			for (auto& input : resources.stage_inputs)
			{
				uint32_t location = reflector.get_decoration(input.id, spv::DecorationLocation);
				const auto& type = reflector.get_type(input.type_id);

				ShaderReflection::VertexInput vi;
				vi.location = location;
				vi.name = reflector.get_name(input.id);  // 实例名
				vi.format = SPIRTypeToRHIFormat(type);
				vi.offset = offset;
				vi.size = GetRHIFormatByteSize(vi.format);
				offset += vi.size;
				reflection.vertexInputs.push_back(vi);
			}
		}

		// ============================================================
		// UBO（Uniform Buffer）
		// ============================================================
		for (auto& ubo : resources.uniform_buffers)
		{
			ShaderReflection::UniformBuffer ub;
			ub.set = reflector.get_decoration(ubo.id, spv::DecorationDescriptorSet);
			ub.binding = reflector.get_decoration(ubo.id, spv::DecorationBinding);
			ub.name = reflector.get_name(ubo.id);  // 实例名（不是类型名！）
			ub.type = (ub.name.find("dyn_") == 0) ?
				EDescriptorType::DynamicUniformBuffer :
				EDescriptorType::UniformBuffer;

			const auto& type = reflector.get_type(ubo.type_id);
			ub.size = reflector.get_declared_struct_size(type);
			ub.arraySize = GetArraySize(type);

			for (uint32_t i = 0; i < type.member_types.size(); i++)
			{
				ShaderReflection::UniformMember member;
				member.name = reflector.get_member_name(ubo.base_type_id, i);
				member.offset = reflector.type_struct_member_offset(type, i);
				member.size = reflector.get_declared_struct_member_size(type, i);
				ub.members.push_back(member);
			}
			reflection.uniformBuffers.push_back(ub);
		}

		// ============================================================
		// SSBO（Storage Buffer）
		// ============================================================
		for (auto& sbo : resources.storage_buffers)
		{
			ShaderReflection::UniformBuffer sb;
			sb.set = reflector.get_decoration(sbo.id, spv::DecorationDescriptorSet);
			sb.binding = reflector.get_decoration(sbo.id, spv::DecorationBinding);
			sb.name = reflector.get_name(sbo.id);  // 实例名
			sb.type = (sb.name.find("dyn_") == 0) ?
				EDescriptorType::DynamicStorageBuffer :
				EDescriptorType::StorageBuffer;

			const auto& type = reflector.get_type(sbo.type_id);
			sb.size = reflector.get_declared_struct_size(type);
			sb.arraySize = GetArraySize(type);

			for (uint32_t i = 0; i < type.member_types.size(); i++)
			{
				ShaderReflection::UniformMember member;
				member.name = reflector.get_member_name(sbo.base_type_id, i);
				member.offset = reflector.type_struct_member_offset(type, i);
				member.size = reflector.get_declared_struct_member_size(type, i);
				sb.members.push_back(member);
			}
			reflection.uniformBuffers.push_back(sb);
		}

		// ============================================================
		// Storage Image（image2D）
		// ============================================================
		for (auto& img : resources.storage_images)
		{
			ShaderReflection::TextureBinding tb;
			tb.set = reflector.get_decoration(img.id, spv::DecorationDescriptorSet);
			tb.binding = reflector.get_decoration(img.id, spv::DecorationBinding);
			tb.name = reflector.get_name(img.id);  // 实例名
			tb.type = EDescriptorType::StorageImage;

			const auto& type = reflector.get_type(img.type_id);
			tb.arraySize = GetArraySize(type);

			reflection.textures.push_back(tb);
		}

		// ============================================================
		// Combined Image Sampler（sampler2D）
		// ============================================================
		for (auto& tex : resources.sampled_images)
		{
			ShaderReflection::TextureBinding tb;
			tb.set = reflector.get_decoration(tex.id, spv::DecorationDescriptorSet);
			tb.binding = reflector.get_decoration(tex.id, spv::DecorationBinding);
			tb.name = reflector.get_name(tex.id);  // 实例名
			tb.type = EDescriptorType::CombinedImageSampler;

			const auto& type = reflector.get_type(tex.type_id);
			tb.arraySize = GetArraySize(type);

			reflection.textures.push_back(tb);
		}

#ifdef ENABLE_VULKAN_API
		// ============================================================
		// Sampler（独立采样器）
		// ============================================================
		for (auto& samp : resources.separate_samplers)
		{
			ShaderReflection::TextureBinding tb;
			tb.set = reflector.get_decoration(samp.id, spv::DecorationDescriptorSet);
			tb.binding = reflector.get_decoration(samp.id, spv::DecorationBinding);
			tb.name = reflector.get_name(samp.id);  // 实例名
			tb.type = EDescriptorType::Sampler;

			const auto& type = reflector.get_type(samp.type_id);
			tb.arraySize = GetArraySize(type);

			reflection.textures.push_back(tb);
		}

		// ============================================================
		// Separate Image（texture2D）
		// ============================================================
		for (auto& img : resources.separate_images)
		{
			ShaderReflection::TextureBinding tb;
			tb.set = reflector.get_decoration(img.id, spv::DecorationDescriptorSet);
			tb.binding = reflector.get_decoration(img.id, spv::DecorationBinding);
			tb.name = reflector.get_name(img.id);  // 实例名
			tb.type = EDescriptorType::SampledImage;

			const auto& type = reflector.get_type(img.type_id);
			tb.arraySize = GetArraySize(type);

			reflection.textures.push_back(tb);
		}

		// ============================================================
		// Input Attachment（subpassInput）
		// ============================================================
		for (auto& input : resources.subpass_inputs)
		{
			ShaderReflection::TextureBinding tb;
			tb.set = reflector.get_decoration(input.id, spv::DecorationDescriptorSet);
			tb.binding = reflector.get_decoration(input.id, spv::DecorationBinding);
			tb.name = reflector.get_name(input.id);  // 实例名
			tb.type = EDescriptorType::InputAttachment;

			const auto& type = reflector.get_type(input.type_id);
			tb.arraySize = GetArraySize(type);

			reflection.textures.push_back(tb);
		}

		// ============================================================
		// Push Constants
		// ============================================================
		for (auto& pc : resources.push_constant_buffers)
		{
			ShaderReflection::PushConstant pushConst;
			pushConst.stage = stage;
			pushConst.offset = 0;

			const auto& type = reflector.get_type(pc.type_id);
			pushConst.size = reflector.get_declared_struct_size(type);

			for (uint32_t i = 0; i < type.member_types.size(); i++)
			{
				ShaderReflection::UniformMember member;
				member.name = reflector.get_member_name(pc.base_type_id, i);
				member.offset = reflector.type_struct_member_offset(type, i);
				member.size = reflector.get_declared_struct_member_size(type, i);
				pushConst.members.push_back(member);
			}
			reflection.pushConstants.push_back(pushConst);
		}
#endif // ENABLE_VULKAN_API

		return reflection;
	}

	
} // namespace DM::RHI