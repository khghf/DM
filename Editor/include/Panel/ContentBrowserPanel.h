#pragma once
#include "Panel.h"
#include <filesystem>
namespace DM
{
	class ContentBrowserPanel:public Panel
	{
		friend class Editor;
		ContentBrowserPanel();
	public:
		~ContentBrowserPanel();

		std::string GetCurPath()const { return CurPath.string(); }
	protected:
		virtual void Render() override;
		std::filesystem::path CurPath;

	};
}
