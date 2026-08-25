#pragma once
#include "Panel.h"
#include <Editor/Services/AssetScanner.h>
#include <vector>
namespace DM
{
	/// <summary>
	/// 新资源导入提示面板：扫描到新可导入资源时弹出模态框，
	/// 提供"全部导入"与"取消"按钮
	/// </summary>
	class ImportAssetDialogPanel : public Panel
	{
	public:
		/// <summary>
		/// 设置待导入的新资源列表(空列表不弹窗)
		/// </summary>
		void ShowNewAssets(std::vector<AssetScanner::NewAsset> assets);

	protected:
		virtual void Render() override;

	private:
		void ImportAll();

	private:
		std::vector<AssetScanner::NewAsset> m_NewAssets;
		bool m_bImporting = false;
	};
}
