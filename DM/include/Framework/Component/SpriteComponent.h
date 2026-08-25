#pragma once
#include"Component.h"
#include<Foundation/MMM/Reference.h>
#include <Core/AssetManagent/AsetPack/AssetPack.h>   // AssetID(序列化的纹理资产引用)
namespace DM
{
	class Texture2D;
	class DM_API SpriteComponent:public Component
	{
	public:
		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent&) = default;
		SpriteComponent(const Vector4& color) :Color(color) {}
		SpriteComponent(const float r, const float g, const float b, const float a) :SpriteComponent(Vector4(r, g, b, a)) {}
		SpriteComponent(const SPtr<Texture2D>& texture) :m_Sprite(texture) {}
		
		Vector4& operator()() { return Color; }
		const Vector4& operator()()const { return Color; }

	public:
		Vector4 Color{ 1.f,1.f,1.f,1.f };
		SPtr<Texture2D>m_Sprite;			// 运行时资源引用(不注册成员，默认序列化不涉及)
		AssetID m_SpriteAssetId;			// 序列化的纹理资产引用(Editor 赋值，加载后按此恢复 m_Sprite)

		float m_DefaultSize = 0.64f;
	};
	// 声明式组件注册(序列化走 Mirror 默认序列化函数)
	REGISTER_COMPONENT(SpriteComponent);
	// 成员注册：m_Sprite 为运行时缓存不序列化；其余成员走默认序列化遍历。
	// 注意：REGISTER_MEMBER 的 inline static 注册器在 Editor/Release(/OPT:REF)下会被链接器丢弃，
	// 改用 REGISTER_COMPONENT_MEMBERS(运行时由 AutoRegisterComponent<T>::Register() 显式补齐)。
	REGISTER_COMPONENT_MEMBERS(SpriteComponent,
		REGISTER_COMPONENT_MEMBER(SpriteComponent, Color);
		REGISTER_COMPONENT_MEMBER(SpriteComponent, m_SpriteAssetId);
		REGISTER_COMPONENT_MEMBER(SpriteComponent, m_DefaultSize);
	);
}
