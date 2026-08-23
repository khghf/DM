#pragma once
#include <vector>
#include <Core/Core.h>
namespace DM { class Layer; }

#include "Layer.h"

namespace DM
{
	class DM_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);

		void Update(float deltaTime);
		//void Render();

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }
		size_t LayerCount() const { return m_Layers.size(); }

	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerInsert = 0;
	};
}
