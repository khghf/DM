#pragma once
#include"Event.h"
namespace DM
{
	struct DM_API KeyEvent :public Event
	{
		inline static constexpr EEventType		GetStaticType()				{ return EEventType::Event; }
		virtual EEventType						GetResourceType()const override		{ return GetStaticType(); }
		inline static constexpr EEventCategory	GetStaticCategory()			{ return EEventCategory::KeyEvent; }
		virtual EEventCategory					GetCategory()const override { return GetStaticCategory(); }

		struct EventData
		{
			int key;
			int scancode;
			int action;
			int mods;
		};
		EventData Data;
	};

	struct DM_API KeyClick :public KeyEvent
	{
		inline static constexpr EEventType		GetStaticType() { return EEventType::KeyClick; }
		virtual EEventType						GetResourceType()const override { return GetStaticType(); }
	};

	struct DM_API KeyPress :public KeyClick
	{
		inline static constexpr EEventType		GetStaticType() { return EEventType::KeyPress; }
		virtual EEventType						GetResourceType()const override { return GetStaticType(); }
	};

	struct DM_API KeyRepeat :public KeyClick
	{
		inline static constexpr EEventType		GetStaticType() { return EEventType::KeyRepeat; }
		virtual EEventType						GetResourceType()const override { return GetStaticType(); }
	};

	struct DM_API KeyRelease :public KeyClick
	{
		inline static constexpr EEventType		GetStaticType() { return EEventType::KeyRelease; }
		virtual EEventType						GetResourceType()const override { return GetStaticType(); }
	};

	struct DM_API KeyTyped :public KeyClick
	{
		inline static constexpr EEventType		GetStaticType() { return EEventType::KeyTyped; }
		virtual EEventType						GetResourceType()const override { return GetStaticType(); }

		struct EventData
		{
			unsigned int KeyCode;
		};
		EventData Data;
	};
}
