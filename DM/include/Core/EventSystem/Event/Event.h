#pragma once
#include<string_view>
#include<Core/Core.h>
namespace DM
{
	enum class EEventType
	{
		None,Event,
		MouseClick,MousePress,MouseRelease,MouseMove,MouseScroll,
		KeyClick,KeyPress,KeyRelease,KeyRepeat,KeyTyped,
		WindowResize,WindowClose,
	};
	enum class EEventCategory
	{
		None,
		Event,
		MouseEvent,
		KeyEvent,
		WindowEvent
	};
	class DM_API  Event
	{
	public:
		virtual ~Event() = default;
		struct FEventData {};
		FEventData Data;
		std::string_view Name = "Event";
		bool bHandled = false;

		template<class EventClass>
		typename const EventClass::FEventData* GetData()const;
 		inline static constexpr EEventType GetStaticType(){return Type;}
		virtual EEventType GetType()const = 0;
		inline static constexpr EEventCategory GetStaticCategory(){ return EEventCategory::Event; }
		virtual EEventCategory GetCategory()const{ return GetStaticCategory(); }
		bool IsInCategory(EEventCategory category)const { return category == GetCategory(); }
	protected:
		static constexpr EEventType Type = EEventType::Event;
	};
}


