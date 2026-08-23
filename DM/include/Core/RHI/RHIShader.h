#pragma once
#include"Core/RHI/RHIResource.h"
namespace DM::RHI
{
	
	class DM_API RHIShader : public RHIResource
	{
	public:
		virtual ~RHIShader() = default;

		EResourceType GetResourceType() const override { return EResourceType::Shader; }
		EShaderStage GetStage()const { return m_Stage; }
		const ShaderReflection& GetReflectionInfo()const { return m_ReflectionInfo; }

	protected:
		RHIShader() = default;
		ShaderReflection m_ReflectionInfo;
		EShaderStage m_Stage;
	};

} // namespace DM::RHI
