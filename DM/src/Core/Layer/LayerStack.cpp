#include "DMPCH.h"
#include <Core/Layer/LayerStack.h>

namespace DM
{
	LayerStack::LayerStack()
	{
		m_LayerInsert = 0;
	}
	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
		{
			if (layer) 
			{
				layer->OnDetach();
				delete layer;
			}
		}
		m_Layers.clear();
		m_LayerInsert = 0;
	}
	void LayerStack::PushLayer(Layer* layer)
	{
		if (!layer) return; 
		m_Layers.emplace(m_Layers.begin()+m_LayerInsert, layer);
		++m_LayerInsert; 
		layer->OnAttach();
	}
	
	void LayerStack::PopLayer(Layer* layer)
	{
		if (!layer || m_Layers.empty()) return;
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it != m_Layers.end())
		{
			layer->OnDetach();
			m_Layers.erase(it);
			--m_LayerInsert;
			delete layer;
		}
	}
	
	void LayerStack::Update(float DeletaTime)
	{
		for (const auto layer : m_Layers)
		{
			layer->OnUpdate(DeletaTime);
		}
	}
}