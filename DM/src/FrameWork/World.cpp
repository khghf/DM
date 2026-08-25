#include "DMPCH.h"
#include<Framework/Base/World.h>
#include<Framework/Component/Component.h>
#include<Framework/Component/TransformComponent.h>
#include<Framework/Component/SpriteComponent.h>

#include<Framework/Base/Entity.h>


namespace DM
{
	REGISTER_TYPE(World);

	World::~World()
	{
	}
	Entity World::CreateEntity()
	{
        Entity en{ m_Registry.create(),this };
		en.AddComponent<TagComponent>("Unnamed Entity");
		en.AddComponent<TransformComponent>();
		en.AddComponent<SpriteComponent>();
		MarkDirty();
        return en;
	}

	void World::DestroyEntity(Entity entity)
	{
		const entt::entity handle = static_cast<entt::entity>(entity);

		// 活实体数 = free_list()（该 entt 实体 storage 为 swap_only 软删设计：
		// destroy 清组件+version bump 并把实体移入 free 区，size() 不会减小）
		const auto liveBefore = m_Registry.storage<entt::entity>().free_list();

		// 完整销毁：移除所有组件 + 释放实体句柄
		m_Registry.destroy(handle);

		const auto liveAfter = m_Registry.storage<entt::entity>().free_list();
		LOG_CORE_INFO("[World] DestroyEntity id={} live_before={} live_after={}",
			static_cast<uint32_t>(entt::to_integral(handle)), liveBefore, liveAfter);

		MarkDirty();
	}

}
