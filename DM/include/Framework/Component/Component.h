#pragma once
#include <Foundation/Math/Matrix.h>
#include <Foundation/Math/Vector.h>
#include<Framework/Camera/Camera.h>
#include"Framework/Base/Entity.h"
namespace DM
{
	class DM_API CLASS() Component:public Entity
	{

	};

	struct DM_API TagComponent
	{
		std::string Tag;
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string_view&name):Tag(name){}
	};

	struct DM_API CameraComponent
	{
		Camera*camera=nullptr;
		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(const ECameraType& type) {}
	};


	/*struct NativeScriptComponent
	{
		ScriptableEntity* Inst = nullptr;
		ScriptableEntity*(*InstantiateScript)();
		void(*DestroyScript)();
		template<typename T>
		void Bind()
		{
			InstantiateScript = [this]() {return static_cast<ScriptableEntity*>(new T();) };
			DestroyScript = [this]() {delete Inst; Inst = nullptr; };
		}
	};*/
}
