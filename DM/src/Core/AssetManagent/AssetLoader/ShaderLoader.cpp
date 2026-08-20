#include<Core/AssetManagent/AssetLoader/ShaderLoader.h>
#include<Core/AssetManagent/AssetImporter/ShaderImporter.h>
#include<Core/AssetManagent/AssetObject/Shader.h>
#include<Core/AssetManagent/AsetPack/ShaderPack.h>
#include<Core/RHI/RHI.h>
namespace DM
{
	SPtr<AssetObject>ShaderLoader::Load(std::string_view path)
	{
		ShaderPack pack{};
		AssetUtil::Deserialize(&pack, path);
		return Load(&pack);
	}
	SPtr<AssetObject>ShaderLoader::Load(AssetPack* pack)
	{
		if (CheckSourceFileModified(pack))
		{
			std::string sourceFilePath = pack->GetMeta().m_SourceFilePath;
			delete pack;
			pack=ShaderImporter::Import(sourceFilePath);
			AssetUtil::SerializePack(pack);
		}

		Shader* shader = new Shader();
		ShaderPack* shaderPack = static_cast<ShaderPack*>(pack);

		RHI::RHIShaderDesc desc{};
		desc.Code = shaderPack->m_Code.data();
		desc.CodeSize = shaderPack->m_Code.size() * sizeof(uint32_t);
		desc.Reflection = shaderPack->m_Reflection;
		switch (shaderPack->m_ShaderStage)
		{
		case ShaderPack::Vertex:desc.Stage = RHI::EShaderStage::Vertex;break;
		case ShaderPack::Fragment:desc.Stage = RHI::EShaderStage::Fragment; break;
		case ShaderPack::Geometry:desc.Stage = RHI::EShaderStage::Geometry; break;
		case ShaderPack::Compute:desc.Stage = RHI::EShaderStage::Compute; break;
		case ShaderPack::TessellationControl:desc.Stage = RHI::EShaderStage::TessellationControl; break;
		case ShaderPack::TessellationEvaluation:desc.Stage = RHI::EShaderStage::TessellationEvaluation;break;
		default:
			desc.Stage = RHI::EShaderStage::Unknown;
			break;
		}

		shader->m_RHISHader = RHI::RHIDevice::Get()->CreateShader(desc);

		return CreateSPtr_Raw(shader);
	}

	REGISTER_LOAD_PATH(EAssetType::Shader, ShaderLoader::Load);
	REGISTER_LOAD_PACK(EAssetType::Shader, ShaderLoader::Load);
}