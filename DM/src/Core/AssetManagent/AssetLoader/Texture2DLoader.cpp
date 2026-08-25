#include<Core/AssetManagent/AssetLoader/Texture2DLoader.h>
#include<Core/AssetManagent/AssetImporter/Texture2DImporter.h>
#include<Core/AssetManagent/AsetPack/TexturePack.h>
#include<Core/AssetManagent/AssetObject/Texture2D.h>
#include<Core/AssetManagent/AssetObject/AssetObjectSetter.h>
#include<Core/RHI/RHI.h>
namespace DM
{
	SPtr<AssetObject>Texture2DLoader::Load(std::string_view path)
	{
		TexturePack*pack=new TexturePack();
		AssetUtil::Deserialize(pack, path);
		return Load(pack);
	}
	SPtr<AssetObject>Texture2DLoader::Load(AssetPack* pack)
	{
		
		Texture2D* tex = new Texture2D();
		TexturePack* texPack = static_cast<TexturePack*>(pack);

		tex->m_Width = texPack->m_Width;
		tex->m_Height = texPack->m_Height;
		RHI::RHITextureDesc desc{};
		desc.Width = tex->m_Width;
		desc.Height = tex->m_Height;

		tex->m_RHITexture = RHI::RHIDevice::Get()->CreateTexture(desc, texPack->m_Data.data());

		// 资产对象持有包内 GUID，后续可通过资产库反查源文件路径
		AssetObjectSetter::SetAssetId(tex, pack->GetGUID());

		return CreateSPtr_Raw<Texture2D>(tex);
	}
	REGISTER_LOAD_PATH(EAssetType::Texture2D, Texture2DLoader::Load);
	REGISTER_LOAD_PACK(EAssetType::Texture2D, Texture2DLoader::Load);
}