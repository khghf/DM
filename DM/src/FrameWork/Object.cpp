#include"DMPCH.h"
#include <Core/Object.h>
namespace DM
{
	Object::Object()
	{
		EnableUpdate(true);
	}

	void Object::Update(float DeltaTime)
	{
	}

	void Object::Destroy()
	{

	}

	void Object::OnDestroy()
	{
	}
}
