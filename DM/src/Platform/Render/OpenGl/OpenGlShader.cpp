#include<DMPCH.h>
#include "Platform/Render/OpenGl/OpenGlShader.h"
#include<glad/glad.h>
#include<Tool/Util/Util.h>
#include<Core/Core.h>
#include<regex>
#include<shaderc/shaderc.hpp>
#include<spirv_cross/spirv_cross.hpp>
#include<spirv_cross/spirv_glsl.hpp>
#include<fstream>
#include<Config.h>
namespace DM
{
	static GLenum StringToOpenGlShaderType(const std::string str)
	{
		if (str == "vertex")
		{
			return GL_VERTEX_SHADER;
		}
		else if (str == "fragment")
		{
			return GL_FRAGMENT_SHADER;
		}
		DM_CORE_ASSERT(false,"{}","Unknown shader type!");
		return GL_FALSE;
	}

	static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
	{
		switch (stage)
		{
		case GL_VERTEX_SHADER:   return shaderc_glsl_vertex_shader;
		case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
		}
		DM_CORE_ASSERT(false,"{}", "");
		return (shaderc_shader_kind)0;
	}

	static const char* GLShaderStageToString(GLenum stage)
	{
		switch (stage)
		{
		case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
		case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
		}
		DM_CORE_ASSERT(false, "{}", "");
		return nullptr;
	}

	static std::string GetCacheDirectory()
	{
		return Config::CacheShaderFolder()+"/Opengl";
	}

	static const char* GLShaderStageCachedOpenGLFileExtension(uint32_t stage)
	{
		switch (stage)
		{
		case GL_VERTEX_SHADER:    return ".cached_opengl.vert";
		case GL_FRAGMENT_SHADER:  return ".cached_opengl.frag";
		}
		DM_CORE_ASSERT(false, "{}", "");
		return "";
	}

	static const char* GLShaderStageCachedVulkanFileExtension(uint32_t stage)
	{
		switch (stage)
		{
		case GL_VERTEX_SHADER:    return ".cached_vulkan.vert";
		case GL_FRAGMENT_SHADER:  return ".cached_vulkan.frag";
		}
		DM_CORE_ASSERT(false, "{}", "");
		return "";
	}
	/// <summary>
	/// 判断SPIR-V是否支持当前opengl版本
	/// </summary>
	/// <returns></returns>
	static bool IsGL_ARB_gl_spirv_Supported()
	{
		GLint major = 0, minor = 0;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);
		if (major < 4 || (major == 4 && minor < 6))
		{
			DM_CORE_ASSERT(false, "OpenGL {}.{}{}", major, minor,"< 4.6: SPIR-V not supported");
			return false;
		}
		return true;
	}


	OpenGlShader::OpenGlShader(const std::string_view& glslCodePath)
	{
		IsGL_ARB_gl_spirv_Supported();


		Util::CretaFloderIfNotExist(GetCacheDirectory());
		this->m_AssetPath = glslCodePath;
		this->m_Name = Util::GetFileName(glslCodePath.data(),false);
		const std::string glslCode = Util::ReadFile(this->m_AssetPath);
		std::unordered_map<GLenum, std::string>type_Code = ProcessGLSLCode(glslCode);
		CompileOrGetVulkanBinaries(type_Code);
		CompileOrGetOpenGLBinaries();
		CreateProgram();
	}

	OpenGlShader::OpenGlShader(const std::string_view& name, const std::string_view& vsCode, const std::string_view& fsCode)
	{
		IsGL_ARB_gl_spirv_Supported();

		Util::CretaFloderIfNotExist(GetCacheDirectory());
		this->m_Name = name;
		this->m_AssetPath = std::string(name) + ".glsl";

		std::unordered_map<GLenum, std::string> type_Code;
		if (!vsCode.empty()) type_Code[GL_VERTEX_SHADER] = std::string(vsCode);
		if (!fsCode.empty()) type_Code[GL_FRAGMENT_SHADER] = std::string(fsCode);

		CompileOrGetVulkanBinaries(type_Code);
		CompileOrGetOpenGLBinaries();
		CreateProgram();
	}


	OpenGlShader::~OpenGlShader()
	{
		if (m_Type == GL_PROGRAM)
		{
			glDeleteProgram(this->m_Id);
		}
	}

	bool OpenGlShader::CheckShaderCompStatus()
	{
		const unsigned int& Id = this->m_Id;
		GLint Type;
		glGetShaderiv(Id, GL_SHADER_TYPE, &Type);//查询类型
		GLint Success = GL_FALSE;
		auto PrintLog = [=]() {
			GLint LogLength = 0;
			glGetShaderiv(Id, GL_INFO_LOG_LENGTH, &LogLength);
			char* InfoLog = new char[LogLength + 1];
			glGetShaderInfoLog(Id, LogLength, NULL, InfoLog);
			InfoLog[LogLength] = '\0';
			DM_CORE_ASSERT(false, "{},{}", "ERROR:program link failed\nInfoLog: ", InfoLog);
			delete[] InfoLog;
			};
		if (Type == GL_VERTEX_SHADER)
		{
			glGetShaderiv(Id, GL_COMPILE_STATUS, &Success);//获取编译状态
			if (Success == GL_FALSE)
			{
				PrintLog();
				return false;
			}
		}
		else if (Type == GL_FRAGMENT_SHADER)
		{
			glGetShaderiv(Id, GL_COMPILE_STATUS, &Success);//获取编译状态
			if (Success == GL_FALSE)
			{
				PrintLog();
				return false;
			}
		}
		else
		{
			std::cerr << "UnKnow Shader Type" << std::endl;
			PrintLog();
			return false;
		}
		return true;
	}


	bool OpenGlShader::CheckProgramLinkStatus()
	{
		const unsigned int& Id = this->m_Id;
		GLint Success = GL_FALSE;
		glGetProgramiv(Id, GL_LINK_STATUS, &Success);
		if (Success == GL_FALSE)
		{
			GLint LogLength = 0;
			glGetProgramiv(Id, GL_INFO_LOG_LENGTH, &LogLength);
			char* InfoLog = new char[LogLength + 1];
			glGetProgramInfoLog(Id, LogLength, NULL, InfoLog);
			InfoLog[LogLength] = '\0';
			DM_CORE_ASSERT(false, "{},{}", "ERROR:program link failed\nInfoLog: ", InfoLog);
			delete[] InfoLog;
			GLint attached;
			glGetProgramiv(Id, GL_ATTACHED_SHADERS, &attached);
			std::cout << "附加后着色器数量：" << attached << std::endl;
			return false;
		}
		return true;
	}
	std::unordered_map<GLenum, std::string> OpenGlShader::ProcessGLSLCode(const std::string& glslCode)
	{
		std::string content = glslCode;
		content = std::regex_replace(content, std::regex(R"(/\*[\s\S]*(\*/))",std::regex::nosubs), "");	//删除多行注释  /**/
		content = std::regex_replace(content, std::regex(R"(//.*)", std::regex::nosubs), "");			//删除单行注释  //
		
		std::unordered_map<GLenum, std::string>res;
		std::regex re(R"(#type\s+(vertex|fragment))");
		std::sregex_token_iterator it_code(content.begin(), content.end(), re, -1);
		std::sregex_token_iterator end_code;
		std::sregex_token_iterator it_name(content.begin(), content.end(), re,1);
		while (it_code != end_code)
		{
			if (it_code->str().empty()) 
			{
				++it_code; 
				continue;
			}
			res[StringToOpenGlShaderType(it_name->str())] = it_code->str();
			//LOG_Core_INFO("{}", it_code->str());
			++it_code;
			++it_name;
		}
		return res;
	}

	void OpenGlShader::CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources)
	{

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
		const bool optimize = true;
		if (optimize)options.SetOptimizationLevel(shaderc_optimization_level_performance);
			

		std::string cacheDirectory = GetCacheDirectory();

		auto& shaderData = m_VulkanSPIRV;
		shaderData.clear();
		for (auto&& [stage, source] : shaderSources)
		{
			std::string shaderFilePath = Util::GetFileName(m_AssetPath,false);
			std::string cachedPath = cacheDirectory +"/"+ (shaderFilePath + GLShaderStageCachedVulkanFileExtension(stage));

			//读取编译好的 SPIR-V 二进制代码，如有
			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);
				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				//没有缓存的Vulkan则从GLSL文件编译 SPIR-V 二进制代码
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, GLShaderStageToShaderC(stage), m_AssetPath.c_str(), options);
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					std::string errorstr = module.GetErrorMessage();
					DM_CORE_ASSERT(errorstr.empty(),"{}", errorstr);
					DM_CORE_ASSERT(false, "{}","");
				}
				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());
				//缓存编译好的SPIR-V 二进制代码
				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}

		for (auto&& [stage, data] : shaderData)
			Reflect(stage, data);
	}

	void OpenGlShader::CompileOrGetOpenGLBinaries()
	{
		auto& shaderData = m_OpenGLSPIRV;

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		const bool optimize = false;
		if (optimize)options.SetOptimizationLevel(shaderc_optimization_level_performance);
			

		std::string cacheDirectory = GetCacheDirectory();

		shaderData.clear();
		m_OpenGLSourceCode.clear();
		for (auto&& [stage, spirv] : m_VulkanSPIRV)
		{
			std::string shaderFilePath = Util::GetFileName(m_AssetPath,false);
			std::string cachedPath = cacheDirectory+ "/"+(shaderFilePath + GLShaderStageCachedOpenGLFileExtension(stage));
			//读取编译好的GLSL代码，如有
			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);
				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				//将Vulkan的SPIRV二进制代码编译成GLSL源码
				spirv_cross::CompilerGLSL glslCompiler(spirv);
				m_OpenGLSourceCode[stage] = glslCompiler.compile();
				auto& source = m_OpenGLSourceCode[stage];
				//将GLSL源码编译成适配的SPIRV二进制代码
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, GLShaderStageToShaderC(stage), m_AssetPath.c_str());
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					std::string errorstr = module.GetErrorMessage();
					DM_CORE_ASSERT(errorstr.empty(), "{}", errorstr);
					DM_CORE_ASSERT(false, "{}", "");
				}

				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());
				//缓存编译好的代码
				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}
	}

	void OpenGlShader::CreateProgram()
	{
		GLuint program = glCreateProgram();
		//4.6+版本才支持SPIR_V
		IsGL_ARB_gl_spirv_Supported();
		std::vector<GLuint> shaderIDs;
		for (auto&& [stage, spirv] : m_OpenGLSPIRV)
		{
			GLuint shaderID = glCreateShader(stage);
			shaderIDs.emplace_back(shaderID);
			glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), (GLsizei)(spirv.size() * sizeof(uint32_t)));
			glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
			glAttachShader(program, shaderID);
		}

		glLinkProgram(program);
		m_Id = program;
		CheckProgramLinkStatus();

		for (auto id : shaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}

	}

	void OpenGlShader::Reflect(GLenum stage, const std::vector<uint32_t>& shaderData)
	{
		spirv_cross::Compiler compiler(shaderData);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		LOG_CORE_TRACE("OpenGlShader::Reflect - {0} {1}",GLShaderStageToString(stage), m_AssetPath);
		LOG_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
		LOG_CORE_TRACE("    {0} resources", resources.sampled_images.size());
		LOG_CORE_TRACE("Uniform buffers:");
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			uint32_t bufferSize = (uint32_t)compiler.get_declared_struct_size(bufferType);
			uint32_t binding = (uint32_t)compiler.get_decoration(resource.id, spv::DecorationBinding);
			size_t memberCount = bufferType.member_types.size();
			LOG_CORE_TRACE("  {0}", resource.name);
			LOG_CORE_TRACE("    Size = {0}", bufferSize);
			LOG_CORE_TRACE("    Binding = {0}", binding);
			LOG_CORE_TRACE("    Members = {0}", memberCount);
		}
	}


}

