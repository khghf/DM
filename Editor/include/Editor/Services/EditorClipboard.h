#pragma once
#include "Editor/Services/IClipboard.h"
#include <string>
#include <vector>

namespace DM
{
	/// <summary>
	/// 编辑器资产剪贴板
	/// 由 ServiceRegistry 统一创建与销毁，UI层经IClipboard接口消费，可替换实现。
	/// </summary>
	class EditorClipboard : public IClipboard
	{
	public:
		EditorClipboard() = default;

		using EMode = EClipboardMode;

		using Snapshot = IClipboard::Snapshot;

		// ---- IClipboard ----
		void	Set(const std::vector<std::string>& paths, EMode mode) override;
		void	Clear() override;
		EMode	GetMode() const override { return m_Mode; }
		const std::vector<std::string>& GetPaths() const override { return m_Paths; }
		bool	HasContent() const override { return !m_Paths.empty(); }
		bool	IsCut() const override { return m_Mode == EMode::Cut && !m_Paths.empty(); }
		bool	IsCopy() const override { return m_Mode == EMode::Copy && !m_Paths.empty(); }

		/// <summary>路径是否在剪贴板中(归一化比较，供剪切半透明提示)</summary>
		bool		Contains(const std::string& absPath) const override;
		// ---- IClipboard 快照(供命令撤销/重做恢复) ----
		Snapshot	SnapshotState() const override;
		void		Restore(const Snapshot& snap) override;

	private:
		EMode m_Mode = EMode::None;
		std::vector<std::string> m_Paths;
	};
}
