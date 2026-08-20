#pragma once
#include"Event.h"
#include<Foundation/Math/Vector.h>
namespace DM
{
	struct DM_API Mouse :public Event
	{
		inline static constexpr EEventType		GetStaticType() { return EEventType::Event; }
		virtual EEventType						GetAssetType()const override { return GetStaticType(); }
		inline static constexpr EEventCategory	GetStaticCategory() { return EEventCategory::Mouse; }
		virtual EEventCategory					GetCategory()const override { return GetStaticCategory(); }
	};


	struct DM_API MouseClick :public Mouse
	{
		inline static constexpr EEventType		GetStaticType() { return EEventType::MouseClick; }
		virtual EEventType						GetAssetType()const override { return GetStaticType(); }

		struct EventData
		{
			Vector2 pos;
			int button;
			int action;
			int mods;
		};
		EventData Data;
	};


	struct DM_API MousePress :public MouseClick
	{
		inline static constexpr EEventType		GetStaticType() { return EEventType::MousePress; }
		virtual EEventType						GetAssetType()const override { return GetStaticType(); }

	};


	struct DM_API MouseRelease :public MouseClick
	{
		inline static constexpr EEventType		GetStaticType() { return EEventType::MouseRelease; }
		virtual EEventType						GetAssetType()const override { return GetStaticType(); }
	};


	struct DM_API MouseMove :public Mouse
	{
		inline static constexpr EEventType		GetStaticType() { return EEventType::MouseMove; }
		virtual EEventType						GetAssetType()const override { return GetStaticType(); }

		struct EventData
		{
			Vector2 pos;
		};
		EventData Data;
	};


	struct DM_API MouseScroll :public Mouse
	{
		inline static constexpr EEventType		GetStaticType() { return EEventType::MouseScroll; }
		virtual EEventType						GetAssetType()const override { return GetStaticType(); }

		struct EventData
		{
			Vector2 offset;
		};
		EventData Data;
	};
}
