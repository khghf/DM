#pragma once
#include"Core/RHI/RHIResource.h"
namespace DM::RHI
{
	/// <summary>
	/// 着色器模块抽象。只负责"编译/持有某阶段的着色器字节码"，
	/// 不直接参与绘制；真正生效是在创建 RHIPipeline 时。
	/// </summary>
	class DM_API RHIShader : public RHIResource
	{
	public:
		virtual ~RHIShader() = default;

		/// <summary>资源类型为着色器（覆写基类）。</summary>
		EResourceType GetAssetType() const override { return EResourceType::Shader; }
		EShaderStage GetStage()const { return m_Stage; }
		const ShaderReflection& GetReflectionInfo()const { return m_ReflectionInfo; }

	protected:
		RHIShader() = default;
		ShaderReflection m_ReflectionInfo;
		EShaderStage m_Stage;
	};

} // namespace DM::RHI
