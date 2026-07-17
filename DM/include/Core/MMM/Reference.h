#pragma once
#include<memory>
namespace DM
{
	template<typename T>
	using UPtr = std::unique_ptr<T>;
	template<typename T,typename...Args>
	inline UPtr<T>CreateUPtr(Args&&...arg)
	{
		return std::make_unique<T>(std::forward<Args>(arg)...);
	}
	template<typename T>
	inline UPtr<T>CreateUPtr(T* raw)
	{
		return std::unique_ptr<T>(raw);
	}


	template<typename T>
	using SPtr = std::shared_ptr<T>;
	template<typename T, typename...Args>
	inline SPtr<T>CreateSPtr(Args&&...arg)
	{
		return std::make_shared<T>(std::forward<Args>(arg)...);
	}
	template<typename T>
	inline SPtr<T>CreateSPtr(T*raw)
	{
		return std::shared_ptr<T>(raw);
	}

	template<typename T>
	using WPtr = std::weak_ptr<T>;
	template<typename T>
	inline WPtr<T>CreateWRef(SPtr<T>ref)
	{
		return std::weak_ptr<T>(ref);
	}
}
