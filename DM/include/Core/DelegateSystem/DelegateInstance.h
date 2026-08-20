#pragma once
#include<DMPCH.h>
#include<Foundation/MMM/Reference.h>
namespace DM
{
	template<typename Ret, typename...Args>
	class IDelegateInstance
	{
	public:
		~IDelegateInstance() = default;
		virtual Ret Execute(Args...arg) = 0;
		virtual IDelegateInstance<Ret, Args...>* Clone() = 0;
	};
	//普通函数
	template<typename Ret, typename...Args>
	class FunDelegateInst :public IDelegateInstance<Ret, Args...>
	{
	private:
		using FunType = Ret(*)(Args...);
		FunType Fun;
	public:
		FunDelegateInst(FunType Fun) :Fun(Fun) {}
		virtual Ret Execute(Args...arg) override
		{
			return Fun(std::forward<Args>(arg)...);
		}
		virtual IDelegateInstance<Ret, Args...>* Clone()
		{
			return new FunDelegateInst(Fun);
		}
	};
	//成员函数
	template<typename Class, typename Ret, typename...Args>
	class MebFunDelegateInst :public IDelegateInstance<Ret, Args...>
	{
	private:
		std::weak_ptr<Class>Obj = nullptr;
		using FunType = Ret(Class::*)(Args...);
		FunType Fun;
	public:
		MebFunDelegateInst(SPtr<Class>Obj, FunType Fun) :Obj(Obj), Fun(Fun) {}
		virtual Ret Execute(Args...arg) override
		{
			if (const auto& element = Obj.lock())
			{
				return (element.get()->*Fun)(std::forward<Args>(arg)...);
			}
			else
			{
				assert(false && "Delegate:Bound Object was destroyed");
			}
			return Ret{};
		}
		virtual IDelegateInstance<Ret, Args...>* Clone()
		{
			return new MebFunDelegateInst(Obj.lock(), Fun);
		}
	};
	template<typename CallObj, typename Ret, typename...Args>
	class LambdaDelegateInst :public IDelegateInstance<Ret, Args...>
	{
	private:
		CallObj Obj;
	public:
		LambdaDelegateInst(CallObj&& Obj) :Obj(std::forward<CallObj>(Obj)) {}
		virtual Ret Execute(Args...arg) override
		{
			return Obj(std::forward<Args>(arg)...);
		}
		virtual IDelegateInstance<Ret, Args...>* Clone()
		{
			return new LambdaDelegateInst(std::forward<CallObj>(Obj));
		}
	};
}
