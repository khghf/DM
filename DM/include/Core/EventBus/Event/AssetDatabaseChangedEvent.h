#pragma once
#include "Event.h"
#include <Core/AssetManagent/AsetPack/AssetPack.h>

namespace DM
{
	/// <summary>
	/// 资产数据库变更事件。
	/// AssetMetaDatabase 增/删/路径变更时通过 EventBus 广播，
	/// 监听者(如编辑器面板)订阅该事件实现数据驱动刷新，替代每帧轮询。
	/// </summary>
	struct DM_API AssetDatabaseChangedEvent : public Event
	{
		enum class EChangeType
		{
			None = 0,
			Add,			// 新增记录
			Remove,			// 删除记录
			PathChanged,	// 源文件路径变更
		};

		inline static constexpr EEventType		GetStaticType()					{ return EEventType::AssetDatabaseChanged; }
		virtual EEventType						GetResourceType() const override { return GetStaticType(); }
		inline static constexpr EEventCategory	GetStaticCategory()				{ return EEventCategory::Asset; }
		virtual EEventCategory					GetCategory() const override		{ return GetStaticCategory(); }

		struct EventData
		{
			EChangeType ChangeType = EChangeType::None;
			AssetID     AssetId;
			std::string SourceFilePath;
		};
		EventData Data;
	};
}
