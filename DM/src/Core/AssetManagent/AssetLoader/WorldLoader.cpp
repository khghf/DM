#include<Core/AssetManagent/AssetLoader/WorldLoader.h>
#include<Core/AssetManagent/AsetPack/WorldPack.h>
#include<Framework/Base/World.h>
namespace DM
{
	SPtr<AssetObject>WorldLoader::Load(std::string_view packPath)
	{
		WorldPack pack{};
		AssetUtil::Deserialize(&pack, packPath);
		return Load(&pack);
	}

	SPtr<AssetObject>WorldLoader::Load(AssetPack* pack)
	{
		WorldPack* worldPack = static_cast<WorldPack*>(pack);
		World* world = new World();

		world->m_Name = worldPack->m_WorldName;

		return CreateSPtr_Raw<World>(world);
	}
	REGISTER_LOAD_PATH(EAssetType::World, WorldLoader::Load);
	REGISTER_LOAD_PACK(EAssetType::World, WorldLoader::Load);
}