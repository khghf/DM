#pragma once
#include <string_view>
#include <Core/Core.h>

namespace DM
{
	// Layer 是编辑器/游戏功能的可插拔单元（L2 Core 层）。
	//
	// 完整生命周期：
	//   OnAttach → OnUpdate(dt) → [Begin → Render → End] → OnDetach
	// 同时提供 OnRender() 作为新统一渲染入口（逐步从 Begin/Render/End 迁移）。
	class DM_API Layer
	{
	public:
		Layer(std::string_view name);
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnEvent(class Event* const e) {}

		// 新统一渲染入口（推荐）。
		virtual void OnRender() {}

		// 旧版分离渲染接口（保留兼容，逐步迁移到 OnRender）。
		virtual void Begin() {}
		virtual void Render() {}
		virtual void End() {}

		inline std::string_view GetName() const { return m_Name; }

	protected:
		std::string_view m_Name;
	};
}
