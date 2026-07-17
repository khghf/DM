#include "DMPCH.h"
#include<Core/EventSystem/EventMgr.h>
namespace DM
{
	EventMgr::EventMgr()
	{
		Init();
	}
	void EventMgr::Init()
	{
	
	}
	void EventMgr::OnEvent( Event* const e)
	{
		e->bHandled = false;
		Disptchers[EEventType::Event].OnEvent(e);
		const auto& it = Disptchers.find(e->GetType());
		if (it == Disptchers.end()|| e->GetType()== EEventType::Event)return;
		it->second.OnEvent(e);
	}
}


