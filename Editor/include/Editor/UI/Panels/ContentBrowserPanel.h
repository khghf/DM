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
		virtual void HandleEvent(Event* const e) override;
		std::filesystem::path CurPath;

	private:
		void RefreshDirectoryCache();
		/// <summary>以当前缓存顺序为基准构建路径顺序列表(供 SelectionManager 做 Shift 范围选择)</summary>
		std::vector<std::string> BuildOrderedPathList() const;

		std::vector<CachedEntry> m_Cache;
		bool m_NeedRefresh = true;
	};
}
