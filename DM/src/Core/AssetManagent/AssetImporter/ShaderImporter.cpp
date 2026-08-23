#include<Core/AssetManagent/AssetImporter/ShaderImporter.h>
#include<Core/AssetManagent/AsetPack/ShaderPack.h>
#include<Core/RHI/ShaderCompiler.h>
namespace DM
{
	AssetPack* ShaderImporter::Import(std::string_view sourceFilePath)
	{
		ShaderPack* pack = new ShaderPack();

		RHI::ShaderCompiler compiler{};

		std::string extension = AssetUtil::GetExtension(sourceFilePath);
		RHI::EShaderStage stage = RHI::EShaderStage::Unknown;

		if (extension==".vert")		{ stage = RHI::EShaderStage::Vertex;				pack->m_ShaderStage = ShaderPack::Vertex; }
		else if(extension==".frag")	{ stage = RHI::EShaderStage::Fragment;				pack->m_ShaderStage = ShaderPack::Fragment; }
		else if(extension==".geom")	{ stage = RHI::EShaderStage::Geometry;				pack->m_ShaderStage = ShaderPack::Geometry; }
		else if(extension==".comp")	{ stage = RHI::EShaderStage::Compute;				pack->m_ShaderStage = ShaderPack::Compute; }
		else if(extension==".tesc")	{ stage = RHI::EShaderStage::TessellationControl;	pack->m_ShaderStage = ShaderPack::TessellationControl; }
		else if(extension==".tese")	{ stage = RHI::EShaderStage::TessellationEvaluation;pack->m_ShaderStage = ShaderPack::TessellationEvaluation; }

		RHI::CompiledShader result=compiler.CompileGLSLFileToSPV(sourceFilePath.data(), stage);

		if (!result.success) { LOG_CORE_WARN("{},{}", "Compile shader failed:", result.errorLog); return nullptr; }

		pack->m_Reflection = compiler.Reflect(result.spirv, stage);
		pack->m_Code = std::move(result.spirv);


		AssetMetaInfo metaInfo = pack->GenerateMetaInfo(sourceFilePath);

		metaInfo.m_DataSize = pack->m_Code.size() * sizeof(uint32_t);

		AssetPackSetter::SetGuid(pack, AssetID::GenNewID());
		AssetPackSetter::SetMetaInfo(pack, std::move(metaInfo));

		return pack;
	}
}