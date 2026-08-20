#pragma once
#include"AssetPack.h"
#include<Core/RHI/RHITypes.h>
namespace DM
{
	struct DM_API ShaderPack:public AssetPack
	{
		virtual EAssetType GetAssetType() const override { return EAssetType::Shader; }

		enum ShaderType
		{
			Unknown,
			Vertex,
			Fragment,
			Geometry,
			Compute,
			TessellationControl,
			TessellationEvaluation,
		};
		ShaderType m_ShaderStage{};
		std::vector<uint32_t>m_Code{};
		RHI::ShaderReflection m_Reflection;

	};
	REGISTER_CHILD(AssetPack, ShaderPack);
	REGISTER_MEMBER(ShaderPack, m_ShaderStage);
	REGISTER_MEMBER(ShaderPack, m_Code);
	REGISTER_MEMBER(ShaderPack, m_Reflection);

	REGISTER_ENUM_ITEM(ShaderPack::ShaderType::Unknown);
	REGISTER_ENUM_ITEM(ShaderPack::ShaderType::Vertex);
	REGISTER_ENUM_ITEM(ShaderPack::ShaderType::Fragment);
	REGISTER_ENUM_ITEM(ShaderPack::ShaderType::Geometry);
	REGISTER_ENUM_ITEM(ShaderPack::ShaderType::Compute);

}