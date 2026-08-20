#pragma once
#include"DM.h"
#include<Framework/Base/World.h>
namespace DM
{
	class Panel
	{
		friend class Editor;
	public:
		
		virtual ~Panel() = default;
	protected:

		virtual void OnContextChanged() {}

		void SetContext(const SPtr<World>& context) { m_Context = context; OnContextChanged(); }
		virtual void Render() = 0;

		virtual void HandleEvent(Event* const e) {}
	protected:
		SPtr<World>m_Context;
	};
}