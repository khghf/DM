#include "DMPCH.h"
#include<Core/EventBus/EventMgr.h>
namespace DM
{
	EventMgr::EventMgr()
	{
		Init();
	}
	void EventMgr::Init()
	{
	
	}
	void EventMgr::Disptch( Event* const e)
	{
		e->bHandled = false;
		Disptchers[EEventType::Event].Disptch(e);
		const auto& it = Disptchers.find(e->GetResourceType());
		if (it == Disptchers.end()|| e->GetResourceType()== EEventType::Event)return;
		it->second.Disptch(e);
	}
}


