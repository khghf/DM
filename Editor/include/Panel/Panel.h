#pragma once
#include"DM.h"
#include<Framework/World.h>
namespace DM
{
	class Panel
	{
		friend class EditorLayer;
	public:
		
		virtual ~Panel() = default;
	protected:
		void SetContext(const SPtr<World>& context) { m_Context = context; }
		virtual void Render() = 0;
	protected:
		SPtr<World>m_Context;
	};
}