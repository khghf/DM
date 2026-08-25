#pragma once
#include <Foundation/Math/Matrix.h>
#include <Foundation/Math/Vector.h>
#include<Framework/Camera/Camera.h>
#include"Framework/Base/Entity.h"
#include <Core/Reflection/Mirror/include/mirror.h>
#include"ComponentRegistry.h"
namespace DM
{
	class DM_API Component:public Entity
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
		Camera*camera=nullptr;				// 运行期相机对象(指针默认序列化跳过，加载后由上层按 m_Type/m_Position/m_Rotation/m_Fov 重建)
		ECameraType m_Type = ECameraType::Ortho;
		Vector3 m_Position{ 0.f, 0.f, 0.f };
		Vector3 m_Rotation{ 0.f, 0.f, 0.f };
		float m_Fov = 45.f;
		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(const ECameraType& type) : m_Type(type) {}
	};

	// 声明式组件注册：REGISTER_TYPE + entt 适配器(序列化走 Mirror 默认序列化函数)
	REGISTER_COMPONENT(TagComponent);
	REGISTER_COMPONENT(CameraComponent);
	// 成员注册：组件序列化完全由 Mirror 默认序列化驱动(遍历 TypeInfo.Members)，无需 BinarySerializer<T> 特化。
	// 注意：REGISTER_MEMBER 生成的 inline static 注册器在 Editor/Release(/OPT:REF)下会被链接器丢弃，
	// 因此改用 REGISTER_COMPONENT_MEMBERS(运行时由 AutoRegisterComponent<T>::Register() 显式调用补齐)。
	REGISTER_COMPONENT_MEMBERS(TagComponent,
		REGISTER_COMPONENT_MEMBER(TagComponent, Tag);
	);
	REGISTER_COMPONENT_MEMBERS(CameraComponent,
		REGISTER_COMPONENT_MEMBER(CameraComponent, m_Type);
		REGISTER_COMPONENT_MEMBER(CameraComponent, m_Position);
		REGISTER_COMPONENT_MEMBER(CameraComponent, m_Rotation);
		REGISTER_COMPONENT_MEMBER(CameraComponent, m_Fov);
	);


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
