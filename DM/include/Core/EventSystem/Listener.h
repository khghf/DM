#pragma once
#include"Event/Event.h"
namespace DM
{
	class DM_API Listener:public BaseDelegate<void(Event*const)>
	{
		friend class Disptcher;
		friend class EventMgr;
	};
}
