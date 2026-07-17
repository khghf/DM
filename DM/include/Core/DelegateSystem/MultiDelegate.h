#pragma once
#include<DMPCH.h>
#include"BaseDelegate.h"
#include"DelegateHash.h"
#include<Core/Log.h>
#include<Core/MMM/Reference.h>
namespace DM
{
	template<typename FunType, typename T = void>
	class MultiDelegate;
	template<typename Ret, typename...Args, typename T>
	class MultiDelegate<Ret(Args...), T>
	{
	private:
		using FunType = Ret(*)(Args...);
		std::unordered_map<size_t, BaseDelegate<Ret(Args...)>>Delegates;
		std::string_view Name;
	public:
		MultiDelegate() = default;
		//普通函数
		void Add(FunType Fun)
		{
			BaseDelegate<Ret(Args...)>BD;
			BD.Bind(std::forward<FunType>(Fun));
			if (Delegates.find(BD.Key()) != Delegates.end())
			{
				LOG_CORE_INFO("MultiDelegate:repeat add");
				return;
			}
			Delegates[BD.Key()] = std::move(BD);
		}
		//成员函数
		template<typename Class>
		void Add(SPtr<Class>Obj, Ret(Class::* MebFunType)(Args...))
		{
			BaseDelegate<Ret(Args...)>BD;
			BD.Bind(std::forward<SPtr<Class>>(Obj), std::forward<Ret(Class::*)(Args...)>(MebFunType));
			if (Delegates.find(BD.Key()) != Delegates.end())
			{
				LOG_CORE_INFO("MultiDelegate:repeat add");
				return;
			}
			Delegates[BD.Key()] = std::move(BD);
		}
		//Lambda表达式
		template<typename CallObj>
		void Add(CallObj&& Obj)
		{
			BaseDelegate<Ret(Args...)>BD;
			BD.Bind(std::forward<CallObj>(Obj));
			if (Delegates.find(BD.Key()) != Delegates.end())
			{
				LOG_CORE_INFO("MultiDelegate:repeat add");
				return;
			}
			Delegates[BD.Key()] = std::move(BD);
		}
		void Add(BaseDelegate<Ret(Args...)>&&BD)
		{
			if (Delegates.find(BD.Key()) != Delegates.end())
			{
				LOG_CORE_ERROR("MultiDelegate:repeat add. HashKey:{}",BD.Key());
				return;
			}
			Delegates[BD.Key()] = std::move(BD);
		}
		void Remove(BaseDelegate<Ret(Args...)>BD)
		{
			Delegates.erase(BD.Key());
		}
		void BroadCast(Args...arg)
		{
			for (auto it = Delegates.begin(); it != Delegates.end();)
			{
				const auto& delegate = it->second;

				if (delegate.IsValid())
				{
					delegate.Execute(std::forward<Args>(arg)...);
					++it; 
				}
				else
				{
					it = Delegates.erase(it);
					LOG_CORE_INFO("{}: found and removed invalid delegate", Name);
				}
			}
		}
		void SetName(const std::string_view& name)
		{
			Name = name;
		}
		int Count()const
		{
			return Delegates.size();
		}
	};
}
