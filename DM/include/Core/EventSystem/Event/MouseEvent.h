#pragma once
#include"Event.h"
#include<Core/Math/Vector.h>
namespace DM
{
	class DM_API MouseEvent :public Event
	{
	public:
		MouseEvent() = default;
		inline static constexpr EEventCategory GetStaticCategory() { return EEventCategory::MouseEvent; }
		virtual EEventCategory GetCategory()const { return GetStaticCategory(); }
	};
	class DM_API MouseClick :public MouseEvent
	{
	public:
		struct FEventData
		{
			Vector2 pos;
			int button;
			int action;
			int mods;
		};
		FEventData Data;
	public:
		inline static constexpr EEventType GetStaticType() { return Type; }
		virtual EEventType GetType()const override { return GetStaticType(); }
	private:
		static constexpr EEventType Type = EEventType::MouseClick;
	};
	class DM_API MousePress :public MouseClick
	{
	public:
		inline static constexpr EEventType GetStaticType() { return Type; }
		virtual EEventType GetType()const override { return GetStaticType(); }
	private:
		static constexpr EEventType Type = EEventType::MousePress;
	};
	class DM_API MouseRelease :public MouseClick
	{
	public:
		inline static constexpr EEventType GetStaticType() { return Type; }
		virtual EEventType GetType()const override { return GetStaticType(); }
	private:
		static constexpr EEventType Type = EEventType::MouseRelease;
	};
	class DM_API MouseMove :public MouseEvent
	{
	public:
		struct FEventData
		{
			Vector2 pos;
		};
		FEventData Data;
	public:
		inline static constexpr EEventType GetStaticType() { return Type; }
		virtual EEventType GetType()const override { return GetStaticType(); }
	private:
		static constexpr EEventType Type = EEventType::MouseMove;
	};
	class DM_API MouseScroll :public MouseEvent
	{
	public:
		struct FEventData
		{
			Vector2 offset;
		};
		FEventData Data;
	public:
		inline static constexpr EEventType GetStaticType() { return Type; }
		virtual EEventType GetType()const override { return GetStaticType(); }
	private:
		static constexpr EEventType Type = EEventType::MouseScroll;
	};
}
