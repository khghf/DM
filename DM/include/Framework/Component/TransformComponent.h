#pragma once
#include"Component.h"
namespace DM
{
	class DM_API TransformComponent : public Component
	{
	public:
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const Vector3& location) :Location(location) {}

		Matrix4 GetTransform()const;
		
	public:
		Vector3 Location =	{ 0.f,0.f,0.f };
		Vector3 Rotation =	{ 0.f,0.f,0.f };
		Vector3 Scale =		{ 1.f,1.f,1.f };
	};
}