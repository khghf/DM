#include<DMPCH.h>
#include<Core/Core.h>
#include<Core/EventBus/Event/Event.h>
#include<Core/EventBus/Event/KeyEvent.h>
#include<Core/EventBus/Event/MouseEvent.h>
#include<Core/EventBus/Event/WindowEvent.h>
#define EventGetDataSpawn(EventClass) \
template<>\
DM_API const EventClass::EventData*Event::GetData<EventClass>()const\
{\
	const EventClass* e = static_cast<const EventClass*>(this);\
	assert(e->GetResourceType()==EventClass::GetStaticType()&&"不能获取其它类型的数据");\
	return &e->Data;\
}
namespace DM
{
	EventGetDataSpawn(Event);
	//**************************KeyEvent
	EventGetDataSpawn(KeyClick);
	EventGetDataSpawn(KeyPress);
	EventGetDataSpawn(KeyRepeat);
	EventGetDataSpawn(KeyRelease);
	EventGetDataSpawn(KeyTyped);
	//**************************Mouse
	EventGetDataSpawn(MouseClick);
	EventGetDataSpawn(MousePress);
	EventGetDataSpawn(MouseRelease);
	EventGetDataSpawn(MouseMove); 
	EventGetDataSpawn(MouseScroll);
	//**************************Window
	EventGetDataSpawn(WindowResize);
	EventGetDataSpawn(WindowClose);
}