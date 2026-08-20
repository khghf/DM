#pragma once
#include <string_view>
#include <Core/Core.h>

namespace DM
{
	struct Event;
	class DM_API Layer
	{
	public:
		Layer(std::string_view name);
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void HandleEvent(Event* const e) {}


		// 旧版分离渲染接口（保留兼容，逐步迁移到 OnRender）。
		/*virtual void BeginRenderPass() {}
		virtual void Render() {}
		virtual void EndRenderPass() {}*/

		inline std::string_view GetName() const { return m_Name; }

	protected:
		std::string_view m_Name;
	};
}
