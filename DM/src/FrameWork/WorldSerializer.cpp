// WorldSerializer.cpp
// 场景(World)序列化核心：entt::registry <-> 实体快照数组。
//
// 组件分发机制(全运行期，零组件感知)：
//   1. 组件在各自头文件用 REGISTER_COMPONENT(T) 声明式注册：
//      REGISTER_TYPE(T) 提供 Mirror 反射(TypeInfo 自动绑定 Mirror 默认序列化函数)，
//      AutoRegisterComponent<T> 把 TypeInfo + entt 适配器(Has/Get/EmplaceDefault/Remove)写入全局表。
//   2. 序列化：遍历 GetComponentTable()(按类型名有序，跨运行稳定)，Has 判定 -> BinarySerializer 写字节；
//      反序列化：按类型名查表 -> EmplaceDefault(emplace 默认构造) -> BinaryDeserializer 就地填充。
//   3. 组件序列化由 REGISTER_MEMBER 注册的成员驱动(Mirror 默认序列化遍历 TypeInfo.Members)，无需 BinarySerializer<T> 特化。
//   4. 新增组件 = 组件头 REGISTER_COMPONENT + REGISTER_MEMBER，本文件与注册表零改动。
#include <Framework/Base/WorldSerializer.h>
#include <Framework/Base/World.h>
#include <Framework/Component/ComponentRegistry.h>
#include <entt/entt.hpp>
#include <sstream>

namespace DM
{
	namespace
	{
		// 把单个快照的组件列表反序列化到指定实体(组件不存在则默认构造后就地填充)
		void DeserializeEntityComponents(entt::registry& registry, entt::entity entity, const EntitySnapshot& snapshot)
		{
			const auto& table = GetComponentTable();
			for (const auto& compData : snapshot.m_Components)
			{
				auto it = table.find(compData.m_TypeName);
				if (it == table.end())
				{
					LOG_CORE_WARN("[WorldSerializer]unknown component type '{}', ignored", compData.m_TypeName);
					continue;
				}

				// emplace 默认构造 + 就地反序列化(零临时内存、零对齐问题)
				void* component = it->second.EmplaceDefault(registry, entity);
				std::istringstream iss(std::string(compData.m_Data.begin(), compData.m_Data.end()), std::ios::binary);
				it->second.TypeInfo->BinaryDeserializer(&iss, component);
			}
		}
	}

	std::vector<EntitySnapshot> WorldSerializer::SerializeWorld(const World* world)
	{
		std::vector<EntitySnapshot> snapshots;
		if (!world) return snapshots;

		const entt::registry& registry = world->m_Registry;
		const auto& table = GetComponentTable();

		// 遍历所有活实体：entity storage 的 each() 只返回活实体
		// (该 entt 的实体 storage 是 swap_only 软删设计：destroy 后实体仍留在
		// packed 数组(移入 free 区)，直接迭代 *storage 会踩到已删实体)
		if (const auto* storage = registry.storage<entt::entity>())
		{
			for (auto [entity] : storage->each())
			{
				EntitySnapshot snap;
				snap.m_EntityId = static_cast<uint32_t>(entt::to_integral(entity));
				snap.m_Version = static_cast<uint8_t>(entt::to_version(entity));

				for (const auto& [name, entry] : table)
				{
					if (!entry.Has(registry, entity)) continue;

					ComponentData data;
					data.m_TypeName = name;
					std::ostringstream oss(std::ios::binary);
					entry.TypeInfo->BinarySerializer(&oss, entry.Get(registry, entity));
					const std::string bytes = oss.str();
					data.m_Data.assign(bytes.begin(), bytes.end());
					snap.m_Components.push_back(std::move(data));
				}
				snapshots.push_back(std::move(snap));
			}
		}

		LOG_CORE_INFO("[WorldSerializer] SerializeWorld: {} entities", snapshots.size());
		return snapshots;
	}

	bool WorldSerializer::DeserializeWorld(World* world, const std::vector<EntitySnapshot>& snapshots)
	{
		if (!world) return false;

		entt::registry& registry = world->m_Registry;
		registry.clear();   // 重建前清空(保留已注册的组件类型)

		LOG_CORE_INFO("[WorldSerializer] DeserializeWorld: {} entities", snapshots.size());

		using traits = entt::entt_traits<entt::entity>;
		for (const auto& snap : snapshots)
		{
			// 跳过"存在但无组件"的幽灵实体：无任何数据可还原，创建只会留下裸实体
			// (层级面板显示为 Entity，访问组件报 "Entity does not have component!")
			if (snap.m_Components.empty())
			{
				LOG_CORE_WARN("[WorldSerializer] skip ghost entity id={} (no components)", snap.m_EntityId);
				continue;
			}
			// 精确重建实体(索引 + version)，保证引用与顺序稳定
			// construct 参数是 index 与 version；m_EntityId 是完整编码(索引+version)，须先提取 index
			const auto entity = registry.create(traits::construct(
				static_cast<traits::entity_type>(entt::to_entity(snap.m_EntityId)),
				static_cast<traits::version_type>(snap.m_Version)));

			try
			{
				DeserializeEntityComponents(registry, entity, snap);
			}
			catch (const std::exception& e)
			{
				LOG_CORE_ERROR("[WorldSerializer] Failed to restore components for entity {}(id={}, version={}): {}",
					static_cast<uint32_t>(entt::to_integral(entity)), snap.m_EntityId, (uint32_t)snap.m_Version, e.what());
				throw;
			}
		}
		return true;
	}

	EntitySnapshot WorldSerializer::SerializeEntity(const World* world, uint32_t entityId)
	{
		EntitySnapshot snap;
		if (!world) return snap;

		const entt::entity entity = entt::entity(entityId); // entityId 为完整编码(索引+version)
		const entt::registry& registry = world->m_Registry;
		if (!registry.valid(entity)) return snap;

		snap.m_EntityId = static_cast<uint32_t>(entt::to_integral(entity));
		snap.m_Version = static_cast<uint8_t>(entt::to_version(entity));

		const auto& table = GetComponentTable();
		for (const auto& [name, entry] : table)
		{
			if (!entry.Has(registry, entity)) continue;

			ComponentData data;
			data.m_TypeName = name;
			std::ostringstream oss(std::ios::binary);
			entry.TypeInfo->BinarySerializer(&oss, entry.Get(registry, entity));
			const std::string bytes = oss.str();
			data.m_Data.assign(bytes.begin(), bytes.end());
			snap.m_Components.push_back(std::move(data));
		}
		return snap;
	}

	bool WorldSerializer::DeserializeEntity(World* world, const EntitySnapshot& snapshot, uint32_t* outEntityId)
	{
		if (!world) return false;

		entt::registry& registry = world->m_Registry;
		using traits = entt::entt_traits<entt::entity>;
		const entt::entity requested = entt::entity(snapshot.m_EntityId); // 完整编码(索引+version)

		entt::entity entity;
		if (registry.valid(requested))
		{
			// 目标实体仍存在(复制场景)：清空其全部已注册组件后恢复
			entity = requested;
			const auto& table = GetComponentTable();
			for (const auto& [name, entry] : table)
			{
				if (entry.Has(registry, entity))
				{
					entry.Remove(registry, entity);
				}
			}
		}
		else
		{
			// 按快照精确重建(索引+version)；m_EntityId 是完整编码，须先提取 index
			entity = registry.create(traits::construct(
				static_cast<traits::entity_type>(entt::to_entity(snapshot.m_EntityId)),
				static_cast<traits::version_type>(snapshot.m_Version)));
			// 索引已被新实体占用时 create 不会精确命中：销毁未命中的实体，回退到全新实体
			if (entt::to_integral(entity) != snapshot.m_EntityId)
			{
				registry.destroy(entity);
				entity = registry.create();
			}
		}

		DeserializeEntityComponents(registry, entity, snapshot);

		if (outEntityId)
		{
			*outEntityId = static_cast<uint32_t>(entt::to_integral(entity));
		}
		return true;
	}
}
