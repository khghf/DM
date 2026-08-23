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
        return en;
	}

	void World::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

}
