#include<Core/AssetManagent/AssetImporter/WorldImporter.h>
#include<Core/AssetManagent/AsetPack/WorldPack.h>
#include<Foundation/FileSystem.h>
namespace DM
{
	AssetPack* WorldImporter::Import(std::string_view sourceFilePath)
	{
		WorldPack* pack = new WorldPack();
		if (sourceFilePath.empty())
		{
			pack->m_WorldName = "NewWorld";

			AssetID guid = AssetID::GenNewID();
			AssetMetaInfo metaInfo{};
			metaInfo.m_Type = EAssetType::World;
			metaInfo.m_SourceFileLastModifyTime = AssetUtil::TimeNow();
			AssetPackSetter::SetGuid(pack, guid);
			AssetPackSetter::SetMetaInfo(pack, metaInfo);
			return pack;
		}
		else
		{
			AssetUtil::Deserialize(pack, sourceFilePath);
		}
		return pack;
	}
}