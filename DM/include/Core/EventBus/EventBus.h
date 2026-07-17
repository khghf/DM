#pragma once
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <any>
#include <Core/Core.h>

namespace DM
{
	// 类型安全的事件总线（替代旧的 EventMgr::Inst() 单例）。
	// 设计要点：
	//   - 事件不需要继承任何基类，任何 trivially-copyable 的 struct 都可以作为事件。
	//   - 支持“即时分发”（Publish）与“延迟分发”（PublishDeferred + FlushDeferred）。
	//   - 由 Engine 创建并注入到各子系统，而非通过单例全局访问，
	//     因此单元测试中可以注入一个 Mock EventBus。
	class DM_API EventBus
	{
	public:
		EventBus() = default;
		~EventBus() = default;

		// 注册某类型事件的监听器。同一类型可注册多个监听器。
		template<class EventType>
		void Register(std::function<void(const EventType&)> callback)
		{
			const std::type_index key(typeid(EventType));
			m_Immediate[key].push_back(
				[cb = std::move(callback)](const std::any& payload)
				{
					cb(std::any_cast<const EventType&>(payload));
				});
		}

		// 注销某类型事件的全部监听器。
		template<class EventType>
		void Unregister()
		{
			m_Immediate.erase(std::type_index(typeid(EventType)));
			// 延迟队列在 Flush 时按类型过滤，这里无需额外处理。
		}

		// 即时分发：立即调用所有该类型事件的监听器。
		template<class EventType>
		void Publish(const EventType& event)
		{
			const std::type_index key(typeid(EventType));
			auto it = m_Immediate.find(key);
			if (it == m_Immediate.end())
				return;
			for (auto& erased : it->second)
				erased(std::any(static_cast<const EventType&>(event)));
		}

		// 延迟分发：事件进入队列，待 FlushDeferred() 时统一分发。
		// 用于避免在遍历/销毁过程中触发重入式事件。
		template<class EventType>
		void PublishDeferred(const EventType& event)
		{
			m_Deferred.emplace_back(std::type_index(typeid(EventType)), std::make_shared<EventType>(event));
		}

		// 派发所有延迟事件，然后清空队列。
		void FlushDeferred()
		{
			for (auto& [key, payload] : m_Deferred)
			{
				auto it = m_Immediate.find(key);
				if (it == m_Immediate.end())
					continue;
				for (auto& erased : it->second)
					erased(payload.get());
			}
			m_Deferred.clear();
		}

	private:
		using ErasedHandler = std::function<void(const std::any&)>;
		std::unordered_map<std::type_index, std::vector<ErasedHandler>> m_Immediate;
		std::vector<std::pair<std::type_index, std::shared_ptr<void>>> m_Deferred;
	};
}
