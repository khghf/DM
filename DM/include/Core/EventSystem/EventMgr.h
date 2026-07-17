#pragma once
#include<Tool/ISingletion.h>
#include"Event/Event.h"
#include "Event/KeyEvent.h"
#include "Event/MouseEvent.h"
#include "Event/WindowEvent.h"
#include"Disptcher.h"
#include"Listener.h"
#include<Core/MMM/Reference.h>
namespace DM
{
	class DM_API EventMgr:public ISingletion<EventMgr>
	{
		friend class ISingletion<EventMgr>;
	public:
		~EventMgr() = default;
	private:
		EventMgr();
		void Init();
	public:
		void OnEvent(Event* const e);
		template<class EventClass, class Class>
		void Register(SPtr<Class>Obj, void(Class::* MebFunType)( Event* const));
		template<class EventClass>
		void Register(void(*FunType)( Event* const));
		template<class EventClass,class LambdaType>
		void Register(LambdaType&& lam);

		template<class EventClass, class Class>
		void UnRegister(SPtr<Class>Obj, void(Class::* MebFunType)(Event* const));
		template<class EventClass>
		void UnRegister(void(*FunType)(Event* const));
		template<class EventClass, class LambdaType>
		void UnRegister(LambdaType&& lam);
	private:
		template<class EventClass>
		void RegisterInternal(Listener&&Lis);
		template<class EventClass>
		void UnRegisterInternal(Listener Lis);
	private:
		std::unordered_map<EEventType, Disptcher>Disptchers;
	};
	template<class EventClass, class Class>
	inline void EventMgr::Register(SPtr<Class>Obj, void(Class::* MebFunType)( Event* const))
	{
		Listener lis;
		lis.Bind(Obj, MebFunType);
		this->RegisterInternal<EventClass>(std::move(lis));
	}
	template<class EventClass>
	inline void EventMgr::Register(void(*FunType)( Event* const))
	{
		Listener lis;
		lis.Bind(FunType);
		this->RegisterInternal<EventClass>(std::move(lis));
	}
	template<class EventClass,class LambdaType>
	inline void EventMgr::Register(LambdaType&&lam)
	{
		Listener lis;
		lis.Bind(std::forward<LambdaType>(lam));
		this->RegisterInternal<EventClass>(std::move(lis));
	}

	template<class EventClass, class Class>
	inline void EventMgr::UnRegister(SPtr<Class>Obj, void(Class::* MebFunType)(Event* const))
	{
		Listener lis;
		lis.Bind(Obj, MebFunType);
		this->UnRegisterInternal<EventClass>(lis);
	}
	template<class EventClass>
	inline void EventMgr::UnRegister(void(*FunType)(Event* const))
	{
		Listener lis;
		lis.Bind(FunType);
		this->UnRegisterInternal<EventClass>(lis);
	}
	template<class EventClass, class LambdaType>
	inline void EventMgr::UnRegister(LambdaType&& lam)
	{
		Listener lis;
		lis.Bind(std::forward<LambdaType>(lam));
		this->UnRegisterInternal<EventClass>(lis);
	}
	
	template<class EventClass>
	inline void EventMgr::RegisterInternal(Listener&& Lis)
	{
		Disptchers[EventClass::GetStaticType()].AddListener(std::move(Lis));
	}
	template<class EventClass>
	inline void EventMgr::UnRegisterInternal(Listener Lis)
	{
		Disptchers[EventClass::GetStaticType()].RemoveListener(Lis);
	}
}
