#pragma once
#include<Core/DelegateSystem/MultiDelegate.h>
#include"Listener.h"
namespace DM
{
	class DM_API Disptcher
	{
		friend class EventMgr;
		friend class EventManager;
	public:
		Disptcher() = default;
		explicit Disptcher(Event* const e) :m_e(e) {}


		//分发给指定类型事件的单个监听DisptchTarget lambda
		template<class SpecifiedEvent,class LambdaType>
		void DisptchSpecifiedEvent(LambdaType&&Lam)
		{
			DisptchTarget.Bind(std::forward<LambdaType>(Lam));
			if (SpecifiedEvent::GetStaticType() == m_e->GetAssetType())
			{
				DisptchTarget.Execute(m_e);
			}
		}


		//分发给指定类型事件的单个监听DisptchTarget 函数指针
		template<class SpecifiedEvent>
		void DisptchSpecifiedEvent(void(*Fun)(Event*const))
		{
			DisptchTarget.Bind(Fun);
			if (SpecifiedEvent::GetStaticType() == m_e->GetAssetType())
			{
				DisptchTarget.Execute(m_e);
			}
		}
	private:

		//分发给指定类型事件的所有监听EventToMD
		void Disptch(Event* const e)
		{
			MD.BroadCast(e);
		}

		void AddListener(Listener&&l)
		{
			MD.Add(static_cast<BaseDelegate<void(Event* const)>>(l));
		}

		void RemoveListener(Listener l)
		{
			MD.Remove(l);
		}
	private:
		Event* m_e=nullptr;
		BaseDelegate<void(Event* const)>DisptchTarget;
		MultiDelegate<void(Event* const)>MD;
	};
}
