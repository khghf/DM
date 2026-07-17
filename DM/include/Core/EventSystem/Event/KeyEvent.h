#pragma once
#include"Event.h"
namespace DM
{
	class DM_API KeyEvent :public Event
	{
	public:
		KeyEvent() = default;
		inline static constexpr EEventCategory GetStaticCategory() { return EEventCategory::KeyEvent; }
		virtual EEventCategory GetCategory()const { return GetStaticCategory(); }
		struct FEventData
		{
			int key;
			int scancode;
			int action;
			int mods;
		};
		FEventData Data;
	protected:
	};
	class DM_API KeyClick :public KeyEvent
	{
	public:
		inline static constexpr EEventType GetStaticType() {return Type;}
		virtual EEventType GetType()const override{ return GetStaticType(); }
	private:
		static constexpr EEventType Type = EEventType::KeyClick;
	};
	class DM_API KeyPress :public KeyClick
	{
	public:
		inline static constexpr EEventType GetStaticType() { return Type; }
		virtual EEventType GetType()const override { return GetStaticType(); }
	private:
		static constexpr EEventType Type = EEventType::KeyPress;
	};
	class DM_API KeyRepeat :public KeyClick
	{
	public:
		inline static constexpr EEventType GetStaticType() { return Type; }
		virtual EEventType GetType()const override { return GetStaticType(); }
	private:
		static constexpr EEventType Type = EEventType::KeyRepeat;
	};
	class DM_API KeyRelease :public KeyClick
	{
	public:
		inline static constexpr EEventType GetStaticType() { return Type; }
		virtual EEventType GetType()const override { return GetStaticType(); }
	private:
		static constexpr EEventType Type = EEventType::KeyRelease;
	};
	class DM_API KeyTyped :public KeyClick
	{
	public:
		struct FEventData
		{
			unsigned int KeyCode;
		};
		FEventData Data;
		inline static constexpr EEventType GetStaticType() { return Type; }
		virtual EEventType GetType()const override { return GetStaticType(); }
	private:
		static constexpr EEventType Type = EEventType::KeyTyped;
	};
}
