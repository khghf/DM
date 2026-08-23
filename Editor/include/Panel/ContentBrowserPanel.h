#pragma once
#include "Panel.h"
#include <filesystem>
namespace DM
{
	class ContentBrowserPanel : public Panel
	{
		friend class Editor;

		struct CachedEntry
		{
			std::filesystem::path path;   // 完整路径(源文件或目录)
			std::string fileName;         // 显示名称
			bool isDirectory = false;     // 是否为目录
			AssetID assetId;              // 注册资源 ID(文件有效)
		};

		ContentBrowserPanel();
	public:
		~ContentBrowserPanel();

		std::string GetCurPath() const { return CurPath.string(); }
	protected:
		virtual void Render() override;
		std::filesystem::path CurPath;

	private:
		void RefreshDirectoryCache();

		std::vector<CachedEntry> m_Cache;
		bool m_NeedRefresh = true;
		uint64_t m_LastModifyCount = 0;  // 上次刷新时数据库的修改计数，用于自动感知数据变更
	};
}
