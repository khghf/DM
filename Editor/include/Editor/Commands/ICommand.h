#pragma once
#include<string>
#include<vector>
#include<utility>
#include<Framework/Base/WorldSerializer.h>
#include<Core/AssetManagent/AssetObject/AssetObject.h>
#include<Core/AssetManagent/AssetMetaDatabase.h>
#include<Editor/Services/IClipboard.h>
namespace DM
{
	struct ICommand
	{
		virtual bool Execute() = 0;
		virtual void Undo() = 0;

		/// <summary>
		/// 重放命令记录的副作用。默认复用 Execute；交互式/依赖选中状态的命令(如弹对话框、
		/// 重新序列化快照)必须覆写，否则 Redo 会破坏状态或弹出多余交互。
		/// </summary>
		virtual void Redo() { Execute(); }

		const std::string& GetName() const { return m_CmdName; }

	protected:
		std::string m_CmdName;
	};

	struct CmdNewWorld:ICommand
	{
		CmdNewWorld() { m_CmdName="NewWorld"; }
		virtual bool Execute()override;
		virtual void Undo()override;
	private:
		std::string m_Path;
	};

	/// <summary>
	/// 打开场景资产(.world)：记录切换前的场景 AssetID，Undo 时通过资产库反查源路径回到原场景。
	/// </summary>
	struct CmdOpenWorld : ICommand
	{
		CmdOpenWorld() { m_CmdName = "OpenWorld"; }
		virtual bool Execute()override;
		virtual void Undo()override;
		virtual void Redo()override;
	private:
		AssetID m_PrevAssetId;      // 切换前的场景 AssetID，Undo 时反查源路径
		std::string m_OpenedPath;   // 本次打开的场景路径，Redo 时直接重放
	};

	/// <summary>
	/// 保存世界(.world)：直接取当前世界的资产源路径落盘，不进入撤销历史。
	/// </summary>
	struct CmdSaveWorld : ICommand
	{
		CmdSaveWorld() { m_CmdName = "SaveWorld"; }
		virtual bool Execute()override;
		virtual void Undo()override;
	};

	/// <summary>
	/// 进入播放模式：播放切换是动作，不进入撤销历史。
	/// </summary>
	struct CmdPlayMode : ICommand
	{
		CmdPlayMode() { m_CmdName = "PlayMode"; }
		virtual bool Execute()override;
		virtual void Undo()override;
	};

	/// <summary>
	/// 退出播放模式：播放切换是动作，不进入撤销历史。
	/// </summary>
	struct CmdStopPlayMode : ICommand
	{
		CmdStopPlayMode() { m_CmdName = "StopPlayMode"; }
		virtual bool Execute()override;
		virtual void Undo()override;
	};

	/// <summary>
	/// 删除选中实体：Execute 前只保存该实体快照，Undo 时单实体恢复。
	/// </summary>
	struct CmdDeleteEntity : ICommand
	{
		CmdDeleteEntity() { m_CmdName = "DeleteEntity"; }
		virtual bool Execute()override;
		virtual void Undo()override;
		virtual void Redo()override;
	private:
		std::vector<EntitySnapshot> m_EntitySnapshots; // 被删实体快照(多实体，与选中集合一一对应)
		std::vector<uint32_t> m_RestoredEntityIds;     // Undo 恢复出的实际实体身份，Redo 据此删除
	};

	/// <summary>
	/// 复制选中实体：把源实体快照改写到新身份后恢复，Undo 时只销毁复制出的新实体。
	/// </summary>
	struct CmdDuplicateEntity : ICommand
	{
		CmdDuplicateEntity() { m_CmdName = "DuplicateEntity"; }
		virtual bool Execute()override;
		virtual void Undo()override;
		virtual void Redo()override;
	private:
		// 每一条记录：源实体快照 + 复制出的新实体实际身份(entt 完整编码)
		struct DuplicateItem
		{
			EntitySnapshot Source;
			uint32_t NewEntityId = 0;
		};
		std::vector<DuplicateItem> m_Items; // 多实体，与选中集合一一对应
	};

	// ==================== 资产操作命令(内容浏览器) ====================

	/// <summary>
	/// 剪切选中资产：把选中路径放入剪贴板(Cut 模式)，被剪切的资产在内容浏览器中半透明提示。
	/// Undo 恢复原剪贴板状态。
	/// </summary>
	struct CmdCutAsset : ICommand
	{
		CmdCutAsset() { m_CmdName = "CutAsset"; }
		virtual bool Execute()override;
		virtual void Undo()override;
	private:
		IClipboard::Snapshot m_OldSnapshot;
	};

	/// <summary>
	/// 复制选中资产：把选中路径放入剪贴板(Copy 模式)，Undo 恢复原剪贴板状态。
	/// </summary>
	struct CmdCopyAsset : ICommand
	{
		CmdCopyAsset() { m_CmdName = "CopyAsset"; }
		virtual bool Execute()override;
		virtual void Undo()override;
	private:
		IClipboard::Snapshot m_OldSnapshot;
	};

	/// <summary>
	/// 粘贴剪贴板资产到目标目录：
	/// Copy 模式 = 复制出新身份资产；Cut 模式 = 移动资产(源文件/资产包随动、数据库路径同步)。
	/// 目标目录存在同名文件时自动重命名("name (N)")。
	/// </summary>
	struct CmdPasteAsset : ICommand
	{
		explicit CmdPasteAsset(std::string targetDir) : m_TargetDir(std::move(targetDir)) { m_CmdName = "PasteAsset"; }
		virtual bool Execute()override;
		virtual void Undo()override;
		virtual void Redo()override;
	private:
		struct PasteItem
		{
			std::string SrcPath;                 // 源(绝对)
			std::string DstPath;                 // 目标(绝对)
			std::vector<std::string> RegisteredFiles; // 本次注册的新资产源文件(复制模式，供 Undo 移除)
			bool IsMove = false;                 // 移动(剪切粘贴)模式
		};
		std::string m_TargetDir;
		std::vector<PasteItem> m_Items;
	};

	/// <summary>
	/// 复制选中资产：原地生成 "name (N)" 副本并注册为全新身份的新资产。
	/// </summary>
	struct CmdDuplicateAsset : ICommand
	{
		CmdDuplicateAsset() { m_CmdName = "DuplicateAsset"; }
		virtual bool Execute()override;
		virtual void Undo()override;
		virtual void Redo()override;
	private:
		struct DupItem
		{
			std::string SrcPath;
			std::string DstPath;
			std::vector<std::string> RegisteredFiles; // 副本注册的新资产源文件
		};
		std::vector<DupItem> m_Items;
		void ReplayFiles();
	};

	/// <summary>
	/// 删除选中资产：源文件与 .dasset 移入 Cache/Trash 回收站并从数据库移除记录，
	/// Undo 原样恢复(记录、路径、GUID 均还原)。
	/// </summary>
	struct CmdDeleteAsset : ICommand
	{
		CmdDeleteAsset() { m_CmdName = "DeleteAsset"; }
		virtual bool Execute()override;
		virtual void Undo()override;
		virtual void Redo()override;
	private:
		struct DeleteItem
		{
			AssetID Guid;
			AssetRecord Record;      // 原始记录
			std::string SourcePath;  // 源文件原位置(绝对)
			std::string PackPath;    // .dasset 原位置(绝对，World 自包含资产为空)
			std::string TrashSource; // 回收站位置(绝对)
			std::string TrashPack;
		};
		std::vector<DeleteItem> m_Items;
	};

}
