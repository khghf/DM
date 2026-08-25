#include <Editor/Commands/ICommand.h>
#include <Editor/Core/EditorApplication.h>
#include <Editor/Core/EditorContext.h>
#include <Editor/Services/IClipboard.h>
#include <Editor/Services/SelectionManager.h>
#include <Editor/UI/Panels/ContentBrowserPanel.h>
#include <Core/AssetManagent/AssetMetaDatabase.h>
#include <Core/AssetManagent/AssetUtil.h>
#include <Core/AssetManagent/AssetImporter/AssetImporter.h>
#include <Core/AssetManagent/AssetImporter/WorldImporter.h>
#include <Core/AssetManagent/AsetPack/AssetPack.h>
#include <Core/AssetManagent/AsetPack/WorldPack.h>
#include <Core/Config/Paths.h>
#include <Core/Log.h>
#include <chrono>
#include <filesystem>
#include <tuple>

namespace DM
{
namespace
{
	namespace fs = std::filesystem;

	// 在目标目录内生成不重名路径: "name (N).ext"
	fs::path MakeUniquePathInDir(const fs::path& src, const fs::path& targetDir)
	{
		const std::string stem = src.stem().string();
		const std::string ext = src.extension().string();
		for (int n = 1; ; ++n)
		{
			const fs::path candidate = targetDir / (stem + " (" + std::to_string(n) + ")" + ext);
			if (!fs::exists(candidate)) return candidate;
		}
	}

	// 原地副本路径：与源同目录，自动重命名
	fs::path MakeUniqueDuplicatePath(const fs::path& src)
	{
		return MakeUniquePathInDir(src, src.parent_path());
	}

	// 递归复制目录，跳过隐藏文件(含 .AssetDirectoryCache 指纹缓存)
	void CopyDirectoryRecursive(const fs::path& src, const fs::path& dst)
	{
		fs::create_directories(dst);
		for (const auto& entry : fs::directory_iterator(src))
		{
			const std::string fileName = entry.path().filename().string();
			if (!fileName.empty() && fileName[0] == '.') continue;
			if (entry.is_directory())
				CopyDirectoryRecursive(entry.path(), dst / fileName);
			else
				fs::copy_file(entry.path(), dst / fileName, fs::copy_options::overwrite_existing);
		}
	}

	// 以全新身份导入单个文件：跳过 GUID 复用(否则同内容副本会被识别为原资产)，
	// 再强制生成新 GUID 兜底，注册并落盘
	bool ImportAssetAsNew(const std::string& filePath)
	{
		AssetPack* pack = AssetImporter::Import(filePath, false);
		if (!pack) return false;
		AssetPackSetter::SetGuid(pack, AssetID::GenNewID());
		AssetUtil::SerializePack(pack, filePath);
		delete pack;
		return true;
	}

	// 导入目录内全部可导入文件，返回成功注册的文件路径列表
	std::vector<std::string> ImportDirectoryAssets(const fs::path& dir)
	{
		std::vector<std::string> registered;
		if (!fs::exists(dir)) return registered;
		for (auto it = fs::recursive_directory_iterator(dir); it != fs::recursive_directory_iterator(); ++it)
		{
			if (!it->is_regular_file()) continue;
			const std::string file = it->path().string();
			if (!AssetImporter::IsImportable(file)) continue;
			if (ImportAssetAsNew(file)) registered.push_back(file);
		}
		return registered;
	}

	// child 是否位于 parent 目录下(或等于自身)
	bool IsPathUnder(const fs::path& child, const fs::path& parent)
	{
		const std::string cs = child.lexically_normal().generic_string();
		const std::string ps = parent.lexically_normal().generic_string();
		return cs == ps|| (cs.size() > ps.size() && cs.compare(0, ps.size(), ps) == 0 && cs[ps.size()] == '/');
	}

	// 移动资产(文件/目录)到目标路径：物理移动源文件(目录整树移动，.dasset 随行)，
	// 并同步数据库 SourceFilePath / AssetPackPath。失败返回 false(可能处于中间状态)。
	bool MoveAssetPath(const fs::path& src, const fs::path& dst)
	{
		AssetMetaDatabase* db = AssetMetaDatabase::Get();
		try
		{
			if (fs::is_directory(src))
			{
				// 目录：整树移动，内部 .dasset 随目录树一并移动，仅需同步数据库路径前缀
				const fs::path relSrc = AssetMetaDatabase::NormalizePath(src.string());
				const fs::path relDst = AssetMetaDatabase::NormalizePath(dst.string());
				fs::rename(src, dst);

				const std::string oldPre = relSrc.generic_string();
				for (const auto& [guid, record] : db->GetAllRecords())
				{
					const fs::path srcRel = AssetMetaDatabase::NormalizePath(record.SourceFilePath);
					if (!IsPathUnder(srcRel, relSrc)) continue;
					const std::string suffix = srcRel.generic_string().substr(oldPre.size());
					const std::string newSource = relDst.generic_string() + suffix;
					const std::string newPack = relDst.generic_string()
						+ record.AssetPackPath.substr(record.SourceFilePath.size());
					db->OnAssetMoved(guid, newSource, newPack);
				}
			}
			else
			{
				// 单文件：移动源文件 + .dasset，再同步数据库
				const AssetID guid = db->GetAssetIDBySourceFilePath(src.string());
				const AssetRecord* record = db->GetRecordBySourceFilePath(src.string());
				fs::rename(src, dst);

				if (record)
				{
					const bool selfContained = record->AssetPackPath == record->SourceFilePath;
					std::string newPack = dst.string();
					if (!selfContained && record->AssetPackPath.size() > record->SourceFilePath.size()
						&& record->AssetPackPath.compare(0, record->SourceFilePath.size(), record->SourceFilePath) == 0)
					{
						newPack += record->AssetPackPath.substr(record->SourceFilePath.size());
						const fs::path packAbs = Paths::ProjectRoot() / AssetMetaDatabase::NormalizePath(record->AssetPackPath);
						if (fs::exists(packAbs)) fs::rename(packAbs, newPack);
					}
					db->OnAssetMoved(guid, dst.string(), selfContained ? dst.string() : newPack);
				}
			}
			return true;
		}
		catch (const std::exception& e)
		{
			LOG_CORE_ERROR("Move asset failed: {} -> {} ({})", src.string(), dst.string(), e.what());
			return false;
		}
	}
}

	// ==================== 剪切 / 复制(仅设置剪贴板) ====================

	bool CmdCutAsset::Execute()
	{
		SelectionManager* sel = EditorContext::GetService<SelectionManager>();
		const auto& selected = sel->GetSelectedPaths();
		if (selected.empty()) return false;
		m_OldSnapshot = EditorContext::GetService<IClipboard>()->SnapshotState();
		EditorContext::GetService<IClipboard>()->Set(selected, EClipboardMode::Cut);
		return true;
	}
	void CmdCutAsset::Undo()
	{
		EditorContext::GetService<IClipboard>()->Restore(m_OldSnapshot);
	}

	bool CmdCopyAsset::Execute()
	{
		SelectionManager* sel = EditorContext::GetService<SelectionManager>();
		const auto& selected = sel->GetSelectedPaths();
		if (selected.empty()) return false;
		m_OldSnapshot = EditorContext::GetService<IClipboard>()->SnapshotState();
		EditorContext::GetService<IClipboard>()->Set(selected, EClipboardMode::Copy);
		return true;
	}
	void CmdCopyAsset::Undo()
	{
		EditorContext::GetService<IClipboard>()->Restore(m_OldSnapshot);
	}

	// ==================== 粘贴 ====================

	bool CmdPasteAsset::Execute()
	{
		IClipboard* clip = EditorContext::GetService<IClipboard>();
		if (!clip->HasContent()) return false;

		const bool isCut = clip->IsCut();
		m_Items.clear();

		for (const std::string& srcStr : clip->GetPaths())
		{
			const fs::path src(srcStr);
			if (!fs::exists(src)) continue;

			// 剪切粘贴到同一目录 = 原地无操作
			if (isCut && fs::weakly_canonical(src.parent_path()) == fs::weakly_canonical(m_TargetDir))
				continue;

			PasteItem item;
			item.SrcPath = src.string();
			item.IsMove = isCut;
			const fs::path dst = MakeUniquePathInDir(src, m_TargetDir);
			item.DstPath = dst.string();

			if (isCut)
			{
				if (!MoveAssetPath(src, dst)) continue;
			}
			else
			{
				try
				{
					if (fs::is_directory(src)) CopyDirectoryRecursive(src, dst);
					else fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
				}
				catch (const std::exception& e)
				{
					LOG_CORE_ERROR("Paste(duplicate) asset failed: {} -> {} ({})", src.string(), dst.string(), e.what());
					continue;
				}
				if (fs::is_directory(dst)) item.RegisteredFiles = ImportDirectoryAssets(dst);
				else if (ImportAssetAsNew(dst.string())) item.RegisteredFiles.push_back(dst.string());
			}
			m_Items.push_back(std::move(item));
		}

		if (m_Items.empty()) return false;  // 全部失败时保留剪贴板

		clip->Clear();  // 粘贴后清空剪贴板

		SelectionManager* sel = EditorContext::GetService<SelectionManager>();
		sel->ClearPaths();
		for (const auto& it : m_Items) sel->AddPath(it.DstPath);
		return true;
	}

	void CmdPasteAsset::Undo()
	{
		for (auto it = m_Items.rbegin(); it != m_Items.rend(); ++it)
		{
			if (it->IsMove)
			{
				MoveAssetPath(it->DstPath, it->SrcPath);
			}
			else
			{
				for (const std::string& f : it->RegisteredFiles)
					AssetMetaDatabase::Get()->RemoveRecordBySourceFilePath(f);
				std::error_code ec;
				fs::remove_all(it->DstPath, ec);
			}
		}
	}

	void CmdPasteAsset::Redo()
	{
		for (auto& item : m_Items)
		{
			if (item.IsMove)
			{
				MoveAssetPath(item.SrcPath, item.DstPath);
			}
			else
			{
				const fs::path src(item.SrcPath);
				const fs::path dst(item.DstPath);
				if (!fs::exists(src)) continue;
				try
				{
					if (fs::is_directory(src)) CopyDirectoryRecursive(src, dst);
					else fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
				}
				catch (...) { continue; }
				item.RegisteredFiles.clear();
				if (fs::is_directory(dst)) item.RegisteredFiles = ImportDirectoryAssets(dst);
				else if (ImportAssetAsNew(dst.string())) item.RegisteredFiles.push_back(dst.string());
			}
		}
	}

	// ==================== 复制副本(原地 "name (N)") ====================

	void CmdDuplicateAsset::ReplayFiles()
	{
		for (auto& item : m_Items)
		{
			const fs::path src(item.SrcPath);
			const fs::path dst(item.DstPath);
			if (!fs::exists(src)) continue;
			try
			{
				if (fs::is_directory(src)) CopyDirectoryRecursive(src, dst);
				else fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
			}
			catch (const std::exception& e)
			{
				LOG_CORE_ERROR("Duplicate asset failed: {} -> {} ({})", src.string(), dst.string(), e.what());
				continue;
			}
			item.RegisteredFiles.clear();
			if (fs::is_directory(dst)) item.RegisteredFiles = ImportDirectoryAssets(dst);
			else if (ImportAssetAsNew(dst.string())) item.RegisteredFiles.push_back(dst.string());
		}
	}

	bool CmdDuplicateAsset::Execute()
	{
		SelectionManager* sel = EditorContext::GetService<SelectionManager>();
		const auto& selected = sel->GetSelectedPaths();
		if (selected.empty()) return false;

		m_Items.clear();
		for (const std::string& srcStr : selected)
		{
			const fs::path src(srcStr);
			if (!fs::exists(src)) continue;
			DupItem item;
			item.SrcPath = src.string();
			item.DstPath = MakeUniqueDuplicatePath(src).string();
			m_Items.push_back(std::move(item));
		}
		if (m_Items.empty()) return false;

		ReplayFiles();

		sel->ClearPaths();
		for (const auto& it : m_Items) sel->AddPath(it.DstPath);
		return true;
	}

	void CmdDuplicateAsset::Undo()
	{
		for (auto& item : m_Items)
		{
			for (const std::string& f : item.RegisteredFiles)
				AssetMetaDatabase::Get()->RemoveRecordBySourceFilePath(f);
			std::error_code ec;
			fs::remove_all(item.DstPath, ec);
		}
	}

	void CmdDuplicateAsset::Redo()
	{
		ReplayFiles();
		SelectionManager* sel = EditorContext::GetService<SelectionManager>();
		sel->ClearPaths();
		for (const auto& it : m_Items) sel->AddPath(it.DstPath);
	}

	// ==================== 删除 ====================

	bool CmdDeleteAsset::Execute()
	{
		SelectionManager* sel = EditorContext::GetService<SelectionManager>();
		const auto& selected = sel->GetSelectedPaths();
		if (selected.empty()) return false;

		AssetMetaDatabase* db = AssetMetaDatabase::Get();
		const fs::path trashRoot = Paths::CacheDir() / "Trash";
		fs::create_directories(trashRoot);
		m_Items.clear();

		// 收集目标：文件=该记录；目录=目录下全部资产记录
		std::vector<std::pair<AssetID, AssetRecord>> targets;
		for (const std::string& srcStr : selected)
		{
			const fs::path src(srcStr);
			if (!fs::exists(src)) continue;
			if (fs::is_directory(src))
			{
				const fs::path rel = AssetMetaDatabase::NormalizePath(src.string());
				for (const auto& [guid, record] : db->GetAllRecords())
				{
					const fs::path srcRel = AssetMetaDatabase::NormalizePath(record.SourceFilePath);
					if (IsPathUnder(srcRel, rel)) targets.emplace_back(guid, record);
				}
			}
			else
			{
				const AssetID guid = db->GetAssetIDBySourceFilePath(src.string());
				if (const AssetRecord* record = db->GetRecordBySourceFilePath(src.string()))
					targets.emplace_back(guid, *record);
				else
					targets.emplace_back(AssetID::GenNewID(), AssetRecord{ "", src.string() }); // 未注册文件仅删文件
			}
		}

		for (const auto& [guid, record] : targets)
		{
			DeleteItem item;
			item.Guid = guid;
			item.Record = record;
			item.SourcePath = (Paths::ProjectRoot() / AssetMetaDatabase::NormalizePath(record.SourceFilePath)).string();

			const bool selfContained = record.AssetPackPath == record.SourceFilePath || record.AssetPackPath.empty();
			item.PackPath = selfContained
				? std::string()
				: (Paths::ProjectRoot() / AssetMetaDatabase::NormalizePath(record.AssetPackPath)).string();

			// 回收站唯一名：时间戳 + 源文件名
			const std::string trashName =
				std::to_string(std::chrono::system_clock::now().time_since_epoch().count())
				+ "_" + fs::path(item.SourcePath).filename().string();
			item.TrashSource = (trashRoot / trashName).string();
			item.TrashPack = item.TrashSource + ".pack.bak";

			std::error_code ec;
			fs::rename(item.SourcePath, item.TrashSource, ec);
			if (ec)
			{
				LOG_CORE_ERROR("Delete asset failed (move to trash): {} ({})", item.SourcePath, ec.message());
				continue;
			}
			if (!item.PackPath.empty() && fs::exists(item.PackPath))
				fs::rename(item.PackPath, item.TrashPack, ec);

			if (guid.IsValid())
				db->RemoveRecordByGuid(guid);  // 广播 Remove 事件驱动面板刷新

			m_Items.push_back(std::move(item));
		}
		return !m_Items.empty();
	}

	void CmdDeleteAsset::Undo()
	{
		for (auto it = m_Items.rbegin(); it != m_Items.rend(); ++it)
		{
			std::error_code ec;
			fs::rename(it->TrashSource, it->SourcePath, ec);
			if (!it->PackPath.empty() && !it->TrashPack.empty() && fs::exists(it->TrashPack))
				fs::rename(it->TrashPack, it->PackPath, ec);
			if (it->Guid.IsValid())
				AssetMetaDatabase::Get()->AddNewAssetPack(it->Guid, it->Record);  // 恢复原记录(GUID/路径均还原)
		}
	}

	void CmdDeleteAsset::Redo()
	{
		for (auto& item : m_Items)
		{
			std::error_code ec;
			fs::rename(item.SourcePath, item.TrashSource, ec);
			if (!item.PackPath.empty() && fs::exists(item.PackPath))
				fs::rename(item.PackPath, item.TrashPack, ec);
			if (item.Guid.IsValid())
				AssetMetaDatabase::Get()->RemoveRecordByGuid(item.Guid);
		}
	}

	// ==================== 新建世界 ====================

	bool CmdNewWorld::Execute()
	{
		ContentBrowserPanel* contentPanel = Editor::Get()->GetPanel<ContentBrowserPanel>();
		if (!contentPanel) return false;
		const fs::path saveDir = contentPanel->GetCurPath();

		// 在这踩的坑:使用new 在Editor.dll上分配World对象时、在Editor.dll本地生成了一份虚表(为什么会额外生成虚表而不是用的DM.dll?)
		// 导致在序列化时根据虚表获取类型信息时无法获取正确的信息而报错
		AssetPack* pack = static_cast<WorldPack*>(WorldImporter::Import(""));
		if (!pack) return false;

		// 与资源粘贴/复制副本同源的不重名策略："NewWorld (N).world"
		const fs::path savePath = MakeUniquePathInDir("NewWorld" + pack->GetExtension(), saveDir);

		AssetMetaInfo metaInfo = pack->GetMeta();
		metaInfo.m_SourceFilePath = savePath.string();
		AssetPackSetter::SetMetaInfo(pack, metaInfo);
		WorldPack* worldPack = static_cast<WorldPack*>(pack);
		worldPack->m_WorldName = savePath.filename().string(); // 含扩展名(与 FileSystem::GetFileName 一致)
		AssetUtil::SerializePack(pack, savePath.string());

		const auto* record = AssetMetaDatabase::Get()->GetRecordByGuid(pack->GetGUID());
		if (!record)
		{
			delete pack;
			return false;
		}
		// 记录源文件路径(而非包路径)，Undo 时按源文件删除记录与文件
		m_Path = record->SourceFilePath;
		delete pack;
		return true;
	}

	void CmdNewWorld::Undo()
	{
		if (m_Path.empty()) return;

		AssetMetaDatabase::Get()->RemoveRecordBySourceFilePath(m_Path);
		AssetMetaDatabase::Get()->Save();
		std::error_code ec;
		fs::remove(m_Path, ec);
	}
}
