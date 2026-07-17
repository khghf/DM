#pragma once
#include"Event.h"
#include<Core/Math/Vector.h>
struct GLFWwindow;
namespace DM
{
	class DM_API WindowEvent :public Event
	{
	public:
		WindowEvent() = default;
		inline static constexpr EEventCategory GetStaticCategory() { return EEventCategory::WindowEvent; }
		virtual EEventCategory GetCategory()const { return GetStaticCategory(); }
	};
	class DM_API WindowResize :public WindowEvent
	{
	public:
		struct FEventData
		{
			Vector2 size;
		};
		FEventData Data;
	public:
		inline static constexpr EEventType GetStaticType() { return Type; }
		virtual EEventType GetType()const override { return GetStaticType(); }
	private:
		static constexpr EEventType Type = EEventType::WindowResize;
	};
	class DM_API WindowClose :public WindowEvent
	{
	public:
		struct FEventData
		{
			GLFWwindow* window;
		};
		FEventData Data;
	public:
		inline static constexpr EEventType GetStaticType() { return Type; }
		virtual EEventType GetType()const override { return GetStaticType(); }
	private:
		static constexpr EEventType Type = EEventType::WindowClose;
	};
}
