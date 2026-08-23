#pragma once
#include"Event.h"
#include<Foundation/Math/Vector.h>
struct GLFWwindow;
namespace DM
{
	struct DM_API Window :public Event
	{
		inline static constexpr EEventType		GetStaticType() { return EEventType::Event; }
		virtual EEventType						GetResourceType()const { return GetStaticType(); }
		inline static constexpr EEventCategory	GetStaticCategory() { return EEventCategory::Window; }
		virtual EEventCategory					GetCategory()const { return GetStaticCategory(); }
	};


	struct DM_API WindowResize :public Window
	{
		inline static constexpr EEventType		GetStaticType() { return EEventType::WindowResize; }
		virtual EEventType						GetResourceType()const { return GetStaticType(); }

		struct EventData
		{
			Vector2 size;
		};
		EventData Data;
	};


	struct DM_API WindowClose :public Window
	{
		inline static constexpr EEventType		GetStaticType() { return EEventType::WindowClose; }
		virtual EEventType						GetResourceType()const { return GetStaticType(); }

		struct EventData
		{
			GLFWwindow* window;
		};
		EventData Data;
	};
}
