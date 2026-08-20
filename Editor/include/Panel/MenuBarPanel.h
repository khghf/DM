#pragma once
#include "Panel.h"
#include <functional>
#include <filesystem>

namespace DM
{
	class MenuBarPanel : public Panel
	{
		friend class Editor;
		MenuBarPanel();
	public:
	protected:
		virtual void Render() override;
		void RenderFileMenu();


		void OpenWorld(const std::string&path);
	};
}
