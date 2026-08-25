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
	// 声明式组件注册(序列化走 Mirror 默认序列化函数)
	REGISTER_COMPONENT(TransformComponent);
	// 成员注册：默认序列化遍历 Members，无需 BinarySerializer<T> 特化。
	// 注意：REGISTER_MEMBER 的 inline static 注册器在 Editor/Release(/OPT:REF)下会被链接器丢弃，
	// 改用 REGISTER_COMPONENT_MEMBERS(运行时由 AutoRegisterComponent<T>::Register() 显式补齐)。
	REGISTER_COMPONENT_MEMBERS(TransformComponent,
		REGISTER_COMPONENT_MEMBER(TransformComponent, Location);
		REGISTER_COMPONENT_MEMBER(TransformComponent, Rotation);
		REGISTER_COMPONENT_MEMBER(TransformComponent, Scale);
	);
}
