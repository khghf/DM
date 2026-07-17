#include "DMPCH.h"
#include<Framework/World.h>
#include<Framework/Component/Component.h>

#include<Framework/Entity.h>


namespace DM
{
    World::World()
    {
    }
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
