#pragma once
// ============================================================
// ECS 运行时反射注册 — 利用 inline static 在 main() 前自动注册
// 包含此头文件即可激活所有 ECS 类型的反射/序列化支持
// ============================================================

#include"Core/Reflection/Mirror/mirror.h"
#include"Framework/Object.h"
#include"Framework/Entity.h"
#include"Framework/World.h"
#include"Framework/Component/Component.h"
namespace DM
{
	// ===================== 类型注册 =====================
	REGISTER_TYPE(Object);
	REGISTER_TYPE(Entity);
	REGISTER_TYPE(World);
	REGISTER_TYPE(Component);
	REGISTER_TYPE(TransformComponent);
	REGISTER_TYPE(SpriteComponent);
	REGISTER_TYPE(TagComponent);
	REGISTER_TYPE(CameraComponent);

	// ===================== 继承关系 =====================
	REGISTER_CHILD(Object, Entity);
	REGISTER_CHILD(Entity, Component);
	REGISTER_CHILD(Object, World);

	// ===================== Object 成员 =====================
	REGISTER_PRIVATE_MEMBER(Object, test);
	REGISTER_PRIVATE_MEMBER(Object, bEnableUpdate);

	// ===================== Entity 成员 =====================
	REGISTER_PRIVATE_MEMBER(Entity, m_Handle);
	REGISTER_PRIVATE_MEMBER(Entity, m_Context);

	// ===================== World 成员 =====================
	REGISTER_MEMBER(World, m_Name);
	REGISTER_MEMBER(World, m_path);
	REGISTER_MEMBER(World, bHasSavedToLocal);
	REGISTER_PRIVATE_MEMBER(World, m_Registry);

	// ===================== TransformComponent 成员 =====================
	REGISTER_MEMBER(TransformComponent, Location);
	REGISTER_MEMBER(TransformComponent, Rotation);
	REGISTER_MEMBER(TransformComponent, Scale);

	// ===================== SpriteComponent 成员 =====================
	REGISTER_MEMBER(SpriteComponent, Color);

	// ===================== TagComponent 成员 =====================
	REGISTER_MEMBER(TagComponent, Tag);

	// ===================== CameraComponent 成员 =====================
	REGISTER_MEMBER(CameraComponent, camera);
}

