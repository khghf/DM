#pragma once
#include <vector>
#include <Core/Core.h>
namespace DM { class Layer; }

#include "Layer.h"

namespace DM
{
	// 管理一组 Layer（L2 Core 层基础服务）。
	// 每帧：OnUpdate(dt) → Begin → Render → End（旧生命周期）
	// 同时支持 OnRender() 新接口（逐步迁移中）。
	class DM_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		// 更新所有 Layer（含 Begin/Render/End 生命周期）。
		void Update(float deltaTime);
		// 新渲染入口：自底向上 OnRender（上层最后绘制，如 UI）。
		void Render();

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }
		size_t LayerCount() const { return m_Layers.size(); }

	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerInsert = 0;
	};
}
