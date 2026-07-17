#include "DMPCH.h"
#include<Core/EventSystem/EventManager.h>
namespace DM
{
	EventManager::EventManager()
	{
		Init();
	}
	void EventManager::Init()
	{
	
	}
	void EventManager::OnEvent( Event* const e)
	{
		e->bHandled = false;
		Disptchers[EEventType::Event].OnEvent(e);
		const auto& it = Disptchers.find(e->GetType());
		if (it == Disptchers.end()|| e->GetType()== EEventType::Event)return;
		it->second.OnEvent(e);
	}
}


