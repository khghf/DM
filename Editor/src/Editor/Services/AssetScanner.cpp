#include "Editor/Services/AssetScanner.h"
#include <fstream>
#include <filesystem>
#include <cstring>
#include <Core/Config/Paths.h>
#include <Core/AssetManagent/AssetMetaDatabase.h>
#include <Core/AssetManagent/AssetImporter/AssetImporter.h>
#include <Core/AssetManagent/AssetUtil.h>
#include <Core/AssetManagent/AsetPack/AssetPack.h>
#include <Core/Log.h>

namespace DM
{
	namespace
	{
		constexpr const char* FingerprintFileName = ".AssetDirectoryCache";
		constexpr const char* DatabaseFileName = "AssetDatabase.txt";
		constexpr const char* CacheMagic = "DM_ASSET_DIR_CACHE v3\n"; // v2 及旧格式无数据库状态戳，视为无缓存并重建

		bool IsSkippedFile(const std::filesystem::path& path)
		{
			const std::string fileName = path.filename().string();
			if (fileName == FingerprintFileName) return true;    // 扫描器自己的指纹缓存
			if (fileName == DatabaseFileName) return true;       // 数据库文件
			if (!fileName.empty() && fileName[0] == '.') return true; // 隐藏文件
			return false;
		}

		struct CachedState
		{
			std::string DatabaseStamp;   // 上次对比时数据库文件状态戳
			std::string DirFingerprint;  // 上次对比时目录指纹
		};

		CachedState ParseCachedState(const std::string& content)
		{
			CachedState state;
			// 版本头不匹配(旧缓存/损坏)时按无缓存处理，触发一次重建
			if (content.rfind(CacheMagic, 0) != 0) return state;
			const std::string body = content.substr(std::strlen(CacheMagic));
			const size_t nl = body.find('\n');
			if (nl == std::string::npos) return state;
			state.DatabaseStamp = body.substr(0, nl);
			state.DirFingerprint = body.substr(nl + 1);
			return state;
		}
	}

	std::string AssetScanner::ComputeDirFingerprint()
	{
		std::string fingerprint;
		namespace fs = std::filesystem;
		if (!fs::exists(Paths::AssetRoot())) return fingerprint;

		for (auto it = fs::recursive_directory_iterator(Paths::AssetRoot());
			it != fs::recursive_directory_iterator(); ++it)
		{
			if (!it->is_directory()) continue;
			const auto& path = it->path();
			if (IsSkippedFile(path)) continue;

			fingerprint += fs::relative(path, Paths::AssetRoot()).generic_string();
			fingerprint += '|';
			fingerprint += std::to_string(AssetUtil::GetFileLastModifyTimeStamp(path.string()));
			fingerprint += '\n';
		}
		return fingerprint;
	}

	std::string AssetScanner::ComputeDatabaseStamp()
	{
		namespace fs = std::filesystem;
		const fs::path dbPath = Paths::AssetRoot() / DatabaseFileName;
		if (!fs::exists(dbPath)) return "<missing>";
		return std::to_string(AssetUtil::GetFileLastModifyTimeStamp(dbPath.string())) + ":" + std::to_string(fs::file_size(dbPath));
	}

	std::string AssetScanner::LoadCachedFingerprint()
	{
		std::ifstream in(Paths::AssetRoot() / FingerprintFileName, std::ios::binary);
		if (!in) return {};
		const std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
		return ParseCachedState(content).DirFingerprint;
	}

	std::string AssetScanner::LoadCachedDatabaseStamp()
	{
		std::ifstream in(Paths::AssetRoot() / FingerprintFileName, std::ios::binary);
		if (!in) return {};
		const std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
		return ParseCachedState(content).DatabaseStamp;
	}

	void AssetScanner::SaveFingerprint(const std::string& fingerprint)
	{
		std::ofstream out(Paths::AssetRoot() / FingerprintFileName, std::ios::binary | std::ios::trunc);
		if (!out) return;
		out << CacheMagic << ComputeDatabaseStamp() << '\n' << fingerprint;
	}

	std::vector<AssetScanner::NewAsset> AssetScanner::Scan()
	{
		std::vector<NewAsset> result;

		// 第一级：目录级指纹(各目录自身 mtime) + 数据库状态戳。
		// 目录未变且数据库未被删除/重建时才跳过；两者任一变化都需重新对比。
		// 注：数据库文件位于资产根目录，不在目录指纹内，必须单独校验。
		const std::string currentDirFingerprint = ComputeDirFingerprint();
		const std::string currentDbStamp = ComputeDatabaseStamp();
		if (currentDirFingerprint == LoadCachedFingerprint() && currentDbStamp == LoadCachedDatabaseStamp())
		{
			return result;
		}

		// 第二级：目录确实发生了变化，此时才遍历文件与数据库对比，找出新资源

		namespace fs = std::filesystem;
		AssetMetaDatabase* db = AssetMetaDatabase::Get();

		if (fs::exists(Paths::AssetRoot()))
		{
			for (auto it = fs::recursive_directory_iterator(Paths::AssetRoot());
				it != fs::recursive_directory_iterator(); ++it)
			{
				if (!it->is_regular_file()) continue;
				const auto& path = it->path();
				if (IsSkippedFile(path)) continue;

				const std::string filePath = path.string();
				// 仅关注可导入的资源文件
				if (!AssetImporter::IsImportable(filePath)) continue;
				// 已在数据库注册的资源跳过
				if (db->GetRecordBySourceFilePath(filePath)) continue;

				result.push_back({ filePath, path.filename().string() });
			}
		}

		// 无论是否发现新资源，都更新指纹，避免未变化时重复全量对比
		SaveFingerprint(currentDirFingerprint);
		return result;
	}

	bool AssetScanner::Import(const std::string& sourceFilePath)
	{
		AssetMetaDatabase* db = AssetMetaDatabase::Get();

		// 决定是否复用已有 GUID：
		// 1. 源路径已注册(重复导入/时间戳更新)          -> 复用身份，保持引用稳定
		// 2. 未注册但内容哈希命中已有记录：
		//    - 原路径文件已不存在(外部重命名/移动)      -> 复用身份，场景引用不失效
		//    - 原路径文件仍存在(复制粘贴出的同内容副本) -> 生成全新身份，否则 GUID 撞车
		//      会被 AddNewAssetPack 去重跳过，导致新文件不注册、面板不刷新
		// 3. 全新资产                                  -> 由导入器生成新 GUID
		bool reuseGuid = db->GetRecordBySourceFilePath(sourceFilePath) != nullptr;
		if (!reuseGuid)
		{
			const AssetID hitId = db->GetAssetIDBySourceFileContent(sourceFilePath);
			if (hitId.IsValid())
			{
				const std::string originalPath = db->GetSourceFilePathByGuid(hitId); // 绝对路径
				reuseGuid = originalPath.empty() || !std::filesystem::exists(originalPath);
			}
		}

		AssetPack* pack = AssetImporter::Import(sourceFilePath, reuseGuid);
		if (!pack)
		{
			LOG_CORE_ERROR("Import asset failed: {}", sourceFilePath);
			return false;
		}

		// 落盘 .dasset 并注册数据库(内部 AddNewAssetPack + Save + 广播事件)
		AssetUtil::SerializePack(pack, sourceFilePath);
		delete pack;
		return true;
	}

	bool AssetScanner::IsSourceFileModified(std::string_view sourceFilePath)
	{
		AssetMetaDatabase* database = AssetMetaDatabase::Get();
		const uint64_t lastModifiedTime = AssetUtil::GetFileLastModifyTimeStamp(sourceFilePath);

		if (const AssetRecord* record = database->GetRecordBySourceFilePath(sourceFilePath))
		{
			// 时间戳未变 -> 未修改
			if (lastModifiedTime == record->LastModifyTime) return false;
			// 时间戳变了但内容哈希一致 -> 仅 touch，不算修改
			if (AssetUtil::Sha256FileContent(sourceFilePath) == record->SourceFileContentHash) return false;
		}
		return true;
	}
}
