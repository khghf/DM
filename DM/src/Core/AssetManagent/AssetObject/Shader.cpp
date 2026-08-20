#include<Core/AssetManagent/AssetObject/Shader.h>
#include<Core/RHI/RHIShader.h>
namespace DM
{
	Shader::~Shader()
	{
		if (m_RHISHader)delete m_RHISHader;
	}
}