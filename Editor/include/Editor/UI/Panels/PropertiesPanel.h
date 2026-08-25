#pragma once
#include "Panel.h"
#include "Framework/Base/Entity.h"

namespace DM
{
	/// <summary>
	/// 属性面板：编辑当前选中实体的组件属性。
	/// </summary>
	class PropertiesPanel : public Panel
	{
		friend class Editor;

		PropertiesPanel() {};

	protected:
		virtual void Render() override;

	private:
		void DrawComponents(const Entity& entity);
	};
}
