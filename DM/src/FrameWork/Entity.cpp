#include "DMPCH.h"
#include<Framework/Base/Entity.h>
#include<Framework/Base/World.h>
#include<entt/entt.hpp>
#include<Framework/Component/Component.h>
namespace DM
{


	Entity::Entity(entt::entity entityHandle, World* context) :m_Handle(entityHandle), m_Context(context)
	{

	}


	void Entity::UpdateData(float DeltaTime)
	{
	}
	void Entity::Destroy()
	{
	}
	void Entity::OnDestroy()
	{
	}

	

}
