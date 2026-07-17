#include<DMPCH.h>
#include "Core/Render/Shader.h"
#include<Core/RHI/RHIDevice.h>
namespace DM
{
	Shader::Shader()
	{

	}
	Shader::~Shader()
	{
	}
	SPtr<Shader> Shader::Create(const std::string_view& name, const std::string_view& vsCode, const std::string_view& fsCode)
	{
		return RHI::RHIDevice::Get().CreateShader(name, vsCode, fsCode);
	}
	SPtr<Shader> Shader::Create(const std::string_view& glslCodePath)
	{
		return RHI::RHIDevice::Get().CreateShader(glslCodePath);
	}
}

