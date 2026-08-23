#pragma once
#include"Component.h"
#include<Foundation/MMM/Reference.h>
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
		SPtr<Texture2D>m_Sprite;

		float m_DefaultSize = 0.64f;
	};
}