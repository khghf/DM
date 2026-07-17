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
	class  EventManager:public ISingletion<EventManager>
	{
		friend class ISingletion<EventManager>;
	public:
		~EventManager() = default;
	private:
		EventManager();
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
		//UnOrderedMap<EEventCategory, UnOrderedMap<EEventType, Disptcher>>CategoryTo;
		std::unordered_map<EEventType, Disptcher>Disptchers;
	};
	template<class EventClass, class Class>
	inline void EventManager::Register(SPtr<Class>Obj, void(Class::* MebFunType)( Event* const))
	{
		Listener lis;
		lis.Bind(Obj, MebFunType);
		this->RegisterInternal<EventClass>(std::move(lis));
	}
	template<class EventClass>
	inline void EventManager::Register(void(*FunType)( Event* const))
	{
		Listener lis;
		lis.Bind(FunType);
		this->RegisterInternal<EventClass>(std::move(lis));
	}
	template<class EventClass,class LambdaType>
	inline void EventManager::Register(LambdaType&&lam)
	{
		Listener lis;
		lis.Bind(std::forward<LambdaType>(lam));
		this->RegisterInternal<EventClass>(std::move(lis));
	}

	template<class EventClass, class Class>
	inline void EventManager::UnRegister(SPtr<Class>Obj, void(Class::* MebFunType)(Event* const))
	{
		Listener lis;
		lis.Bind(Obj, MebFunType);
		this->UnRegisterInternal<EventClass>(lis);
	}
	template<class EventClass>
	inline void EventManager::UnRegister(void(*FunType)(Event* const))
	{
		Listener lis;
		lis.Bind(FunType);
		this->UnRegisterInternal<EventClass>(lis);
	}
	template<class EventClass, class LambdaType>
	inline void EventManager::UnRegister(LambdaType&& lam)
	{
		Listener lis;
		lis.Bind(std::forward<LambdaType>(lam));
		this->UnRegisterInternal<EventClass>(lis);
	}
	
	template<class EventClass>
	inline void EventManager::RegisterInternal(Listener&& Lis)
	{
		/*const auto& it = Disptchers.find(EventClass::GetStaticCategory());
		if (it == Disptchers.end())return;
		it->second.AddListener<EventClass>(std::move(Lis));*/
		Disptchers[EventClass::GetStaticType()].AddListener(std::move(Lis));
	}
	template<class EventClass>
	inline void EventManager::UnRegisterInternal(Listener Lis)
	{
		/*const auto& it = Disptchers.find(EventClass::GetStaticCategory());
		if ( it== Disptchers.end())return;
		it->second.RemoveListener<EventClass>(Lis);*/
		Disptchers[EventClass::GetStaticType()].RemoveListener(Lis);
	}
}
