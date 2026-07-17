#include"DMPCH.h"
#include <Framework/Object.h>
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
