#pragma once
#include<Resource/Asset.h>
#include<Core/Render/Shader.h>
#include<glad/glad.h>
#include<Core/Math/Matrix.h>
#include<glm/gtc/type_ptr.hpp>
namespace DM
{
	//enum class EShaderType :uint8_t//着色器类型
	//{
	//	None,
	//	VS,//顶点着色器
	//	FS,//片段着色器
	//	PS,//着色程序
	//};
	class DM_API OpenGlShader :public Shader
	{
	public:
		OpenGlShader(const std::string_view& glslCodePath);
		OpenGlShader(const std::string_view& name, const std::string_view& vsCode, const std::string_view& fsCode);
		~OpenGlShader();
	
	public:
		inline unsigned int GetID()const { return m_Id; }
		inline GLenum GetShaderType()const { return m_Type; }
		virtual void Bind()override { assert(m_Id && "Bind Invalid Shader Id"); glUseProgram(m_Id); }
		virtual void UnBind()override { assert(m_Id && "UnBind Invalid Shader Id"); glUseProgram(0); }

		virtual void SetInt(const std::string_view& name, const int32_t& val)	override					{ UploadInt(name, val); }
		virtual void SetIntArray(std::string_view name, const int32_t* val, const uint32_t& count)	override{ UploadIntArray(name,val,count); }
		virtual void SetFloat3(const std::string_view& name, const Vector3& val)override					{ UploadFloat3(name, val); }
		virtual void SetFloat4(const std::string_view& name, const Vector4& val)override					{ UploadFloat4(name, val); }
		virtual void SetMat3x3(const std::string_view& name, const Matrix3& val)override					{ UploadMat3x3(name, val); }
		virtual void SetMat4x4(const std::string_view& name, const Matrix4& val)override					{ UploadMat4x4(name, val); }
		
	private:
		inline GLint GetUniformLocation(std::string_view name) { return glGetUniformLocation(m_Id, name.data()); }

		inline void UploadInt(std::string_view name, const GLint& val)							{ glUniform1i(GetUniformLocation(name), val); }
		inline void UploadIntArray(std::string_view name, const GLint* val,const uint32_t&count){ glUniform1iv(GetUniformLocation(name), count,val); }
		inline void UploadFloat3(const std::string_view& name, const Vector3& val)				{ glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(val)); }
		inline void UploadFloat4(const std::string_view& name, const Vector4& val)				{ glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(val)); }
		inline void UploadMat3x3(const std::string_view& name, const Matrix3& val)				{ glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(val)); }
		inline void UploadMat4x4(std::string_view name, const Matrix4& val)						{ glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(val)); }

		bool CheckShaderCompStatus();
		bool CheckProgramLinkStatus();
		std::unordered_map<GLenum, std::string>ProcessGLSLCode(const std::string& glslCode);

		

		/// <summary>
		/// 将源码编译成适配Vulkan的SPIRV二进制代码
		/// </summary>
		/// <param name="shaderSources">shader类型及其源码</param>
		void CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources);

		/// <summary>
		/// 从m_VulkanSPIRV代码反向编译成适配OpenGL的GLSL源码并缓存适配OpenGL的SPIRV二进制代码
		/// </summary>
		void CompileOrGetOpenGLBinaries();

		void CreateProgram();
		void Reflect(GLenum stage, const std::vector<uint32_t>& shaderData);





	private:
		unsigned int m_Id;
		GLenum m_Type;

		std::unordered_map<GLenum, std::vector<uint32_t>> m_VulkanSPIRV;
		std::unordered_map<GLenum, std::vector<uint32_t>> m_OpenGLSPIRV;
		std::unordered_map<GLenum, std::string> m_OpenGLSourceCode;
	};
}


