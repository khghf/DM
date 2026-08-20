#pragma once
#include"Event/Event.h"
namespace DM
{
	class EventBus
	{
	public:
		EventBus();
		~EventBus();
		static void SendEvent(Event& e);
	private:

	};
}