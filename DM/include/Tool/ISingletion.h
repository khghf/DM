#pragma once
#include<memory>
#include<Core/Core.h>
namespace DM
{
	template<typename T>
	class DM_API  ISingletion
	{
	public:
		static T* GetInst()
		{
			if (Instance == nullptr)
			{
				Instance = new T();
			}
			return Instance;
		}
		~ISingletion()
		{
			if (Instance)
			{
				delete Instance;
				Instance = nullptr;
			}
		}
	protected:
		ISingletion() = default;
	private:
		static T* Instance;
		ISingletion(const ISingletion&) = delete;
		ISingletion(ISingletion&) = delete;
		ISingletion(ISingletion&&) = delete;
		ISingletion& operator=(const ISingletion&) = delete;
		ISingletion& operator=(ISingletion&) = delete;
		ISingletion& operator=(ISingletion&&) = delete;
	};
}
