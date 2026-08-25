#include<Core/AssetManagent/AssetLoader/WorldLoader.h>
#include<Core/AssetManagent/AsetPack/WorldPack.h>
#include<Core/AssetManagent/AssetObject/AssetObjectSetter.h>
#include<Framework/Base/World.h>
#include<Framework/Base/WorldSerializer.h>
#include<Core/Log.h>
namespace DM
{
	SPtr<AssetObject>WorldLoader::Load(std::string_view packPath)
	{
		WorldPack pack{};
		AssetUtil::Deserialize(&pack, packPath);
		LOG_CORE_INFO("[WorldLoader] Load world: path={} entities={}", packPath, pack.m_Entities.size());
		return Load(&pack);
	}

	SPtr<AssetObject>WorldLoader::Load(AssetPack* pack)
	{
		WorldPack* worldPack = static_cast<WorldPack*>(pack);
		World* world = new World();

		world->m_Name = worldPack->m_WorldName;

		// 实体快照重建：开发初期不做格式版本兼容，统一按快照数据还原。
		// 旧空壳 .world 没有 m_Entities，反序列化结果即空世界。
		WorldSerializer::DeserializeWorld(world, worldPack->m_Entities);

		// 资产对象持有包内 GUID，后续可通过资产库反查源文件路径
		AssetObjectSetter::SetAssetId(world, pack->GetGUID());

		return CreateSPtr_Raw<World>(world);
	}
	REGISTER_LOAD_PATH(EAssetType::World, WorldLoader::Load);
	REGISTER_LOAD_PACK(EAssetType::World, WorldLoader::Load);
}