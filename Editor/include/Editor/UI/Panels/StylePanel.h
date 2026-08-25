#pragma once
#include "Editor/UI/Panels/Panel.h"

namespace DM
{
	// 样式设置面板：运行时调节颜色/形状并保存外观(Config/Style.json)
	class StylePanel : public Panel
	{
	public:
		virtual void Render() override;

		bool m_Open = false; // 面板可见性(由 Window 菜单切换)
	};
}
