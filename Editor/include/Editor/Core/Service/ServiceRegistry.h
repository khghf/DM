#pragma once
#include "Editor/Core/Service/IService.h"
#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>

namespace DM
{
	/// <summary>
	/// 编辑器服务注册表(组合根)：统一管理服务生命周期(创建 / Init / Shutdown / 销毁)，
	/// </summary>
	class ServiceRegistry
	{
	public:
		template<typename TImpl, typename... Args>
		ServiceRegistry& Register(Args&&... args)
		{
			static_assert(std::is_base_of<IService, TImpl>::value, "TImpl must derive from IService");
			auto service = std::make_unique<TImpl>(std::forward<Args>(args)...);
			m_Services.emplace(std::type_index(typeid(TImpl)), std::move(service));
			return *this;
		}

		template<typename TInterface, typename TImpl, typename... Args>
		ServiceRegistry& RegisterAs(Args&&... args)
		{
			static_assert(std::is_base_of<IService, TImpl>::value, "TImpl must derive from IService");
			static_assert(std::is_base_of<TInterface, TImpl>::value, "TImpl must derive from TInterface");
			auto service = std::make_unique<TImpl>(std::forward<Args>(args)...);
			m_Services.emplace(std::type_index(typeid(TInterface)), std::move(service));
			return *this;
		}

		template<typename T>
		T* Get() const
		{
			auto it = m_Services.find(std::type_index(typeid(T)));
			return it != m_Services.end() ? static_cast<T*>(it->second.get()) : nullptr;
		}

		void InitAll();

		void ShutdownAll();

	private:
		std::unordered_map<std::type_index, std::unique_ptr<IService>> m_Services;
	};
}
