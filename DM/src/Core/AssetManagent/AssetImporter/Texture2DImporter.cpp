#include<Core/AssetManagent/AssetImporter/Texture2DImporter.h>
#include<Core/AssetManagent/AsetPack/TexturePack.h>
namespace DM
{
	AssetPack* Texture2DImporter::Import(std::string_view sourceFilePath)
	{
		TexturePack* pack = new TexturePack();

		int texWidth, texHeight, texChannels;
		stbi_uc* pixel = stbi_load(sourceFilePath.data(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		if (!pixel) {
			std::cout << "stbi failure reason: " << stbi_failure_reason() << std::endl;
			throw std::runtime_error("failed to load texture image!");
			return nullptr;
		}

		AssetID guid = AssetID::GenNewID();

		AssetMetaInfo metaInfo = pack->GenerateMetaInfo(sourceFilePath);

		metaInfo.m_DataSize = 4 * texWidth * texHeight;

		pack->m_Data = { pixel,pixel + metaInfo.m_DataSize };

		pack->m_Width = (uint32_t)texWidth;
		pack->m_Height = (uint32_t)texHeight;
		stbi_image_free(pixel);

		AssetPackSetter::SetGuid(pack, std::move(guid));
		AssetPackSetter::SetMetaInfo(pack, std::move(metaInfo));
		return pack;
	}
}