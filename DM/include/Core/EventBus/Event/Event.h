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
		AssetDatabaseChanged,
	};
	enum class EEventCategory
	{
		None,
		Mouse,
		KeyEvent,
		Window,
		Asset
	};
	struct DM_API  Event
	{
		virtual ~Event() = default;
		struct EventData {};
		EventData Data;
		std::string_view Name = "Event";
		bool bHandled = false;

		template<class EventClass>
		typename const EventClass::EventData* GetData()const;


 		inline static constexpr EEventType		GetStaticType()		{ return EEventType::Event;}
		virtual EEventType						GetResourceType()const		{ return GetStaticType(); }
		inline static constexpr EEventCategory	GetStaticCategory()	{ return EEventCategory::None; }
		virtual EEventCategory					GetCategory()const	{ return GetStaticCategory(); }

		bool IsInCategory(EEventCategory category)const { return category == GetCategory(); }
	};
}


