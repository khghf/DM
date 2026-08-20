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
	STRUCT()
	struct DM_API TransformComponent
	{
		Vector3 Location =	{ 0.f,0.f,0.f };
		Vector3 Rotation =		{ 0.f,0.f,0.f };
		Vector3 Scale =			{ 1.f,1.f,1.f };
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const Vector3& location) :Location(location) {}
		Matrix4 GetTransform()const
		{
				return	glm::translate	(Matrix4(1.f), Location) *
						glm::rotate		(Matrix4(1.f), Rotation.x, { 1.f,0.f,0.f }) *
						glm::rotate		(Matrix4(1.f), Rotation.y, { 0.f,1.f,0.f }) *
						glm::rotate		(Matrix4(1.f), Rotation.z, { 0.f,0.f,1.f }) *
						glm::scale		(Matrix4(1.f), Scale);
		}
	};

	STRUCT()
	struct DM_API SpriteComponent
	{
		Vector4 Color{1.f,1.f,1.f,1.f};
		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent&) = default;
		SpriteComponent(const Vector4& color) :Color(color) {}
		SpriteComponent(const float r, const float g, const float b, const float a) :SpriteComponent(Vector4(r,g,b,a)){}
		Vector4& operator()() { return Color; }
		const Vector4& operator()()const { return Color; }
	};

	STRUCT()
	struct DM_API TagComponent
	{
		std::string Tag;
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string_view&name):Tag(name){}
	};

	STRUCT()
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
