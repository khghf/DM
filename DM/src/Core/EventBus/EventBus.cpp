#include<Core/EventBus/EventBus.h>
#include<Engine.h>
namespace DM
{
	EventBus::EventBus()
	{

	}

	EventBus::~EventBus()
	{

	}

	void EventBus::SendEvent(Event& e)
	{
		Engine::Get()->HandleEvent(&e);
	}
}