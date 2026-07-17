#include "DMPCH.h"
#include<Framework/Entity.h>
#include<Framework/World.h>
#include<entt/entt.hpp>
#include<Framework/Component/Component.h>
namespace DM
{


	Entity::Entity(entt::entity entityHandle, World* context) :m_Handle(entityHandle), m_Context(context)
	{

	}


	void Entity::Update(float DeltaTime)
	{
	}
	void Entity::Destroy()
	{
	}
	void Entity::OnDestroy()
	{
	}

	

}
