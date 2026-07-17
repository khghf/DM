#pragma once
#include<DMPCH.h>
#include"DelegateInstance.h"
#include"DelegateHash.h"
#include<Core/MMM/Reference.h>
namespace DM
{
	template<typename FunType>
	class BaseDelegate;
	template<typename Ret, typename...Args>
	class  BaseDelegate<Ret(Args...)>
	{
		IDelegateInstance<Ret, Args...>*DelegateInstance=nullptr;
		using FunType = Ret(*)(Args...);
		using HashKey = size_t;
		HashKey key;
		std::string_view Name;
	public:
		BaseDelegate() noexcept = default;
		BaseDelegate(BaseDelegate&& other)noexcept
		{
			this->DelegateInstance = other.DelegateInstance;
			this->key = other.key;
			this->Name = other.Name;
			other.DelegateInstance = nullptr;
		}
		BaseDelegate& operator=(BaseDelegate&& other)noexcept
		{
			if (this == &other)return *this;
			if(this->DelegateInstance)delete this->DelegateInstance;
			this->DelegateInstance = other.DelegateInstance;
			other.DelegateInstance = nullptr;
			this->key = other.key;
			this->Name = other.Name;
			return *this;
		}
		BaseDelegate(const BaseDelegate& other)
		{
			if (other.DelegateInstance == nullptr)return;
			this->DelegateInstance = other.DelegateInstance->Clone();
			this->key = other.key;
			this->Name = other.Name;
		}
		BaseDelegate& operator=(const BaseDelegate& other)
		{
			if (this == &other)return *this;
			if (other.DelegateInstance != nullptr)
			{
				if (this->DelegateInstance)delete this->DelegateInstance;
				this->DelegateInstance = other.DelegateInstance->Clone();
				this->key = other.key;
				this->Name = other.Name;
			}
			return *this;
		}
		void Bind(Ret(*Fun)(Args...))
		{
			FFunHash<FunType>hash;
			key = hash(Fun);
			DelegateInstance = new FunDelegateInst<Ret, Args...>(Fun);
		}
		template<typename Class>
		void Bind(SPtr<Class>Obj, Ret(Class::* MebFunType)(Args...))
		{
			key = FMebFunHash<Class, Ret, Args...>()(MebFunType);
			DelegateInstance = new MebFunDelegateInst<Class, Ret, Args...>(Obj, MebFunType);
		}
		template<typename CallObj>
		void Bind(CallObj&& Obj)
		{
			key = FLambdaHash<CallObj>()(Obj);
			//LOG_Core_INFO("{}",key);
			DelegateInstance = new LambdaDelegateInst<CallObj, Ret, Args...>(std::forward<CallObj>(Obj));
		}
		Ret Execute(Args... arg)const
		{
			if (DelegateInstance)
			{
				return DelegateInstance->Execute(std::forward<Args>(arg)...);
			}
			else
			{
				throw std::runtime_error("Delegate is not bound to any callable object");
			}
		}
		bool IsValid()const
		{
			return DelegateInstance == nullptr ? false : true;
		}
		inline HashKey Key()const
		{
			return key;
		}
		void SetName(const std::string_view& name)
		{
			Name = name;
		}
	};
}
