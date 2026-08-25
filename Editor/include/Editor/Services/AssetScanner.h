#pragma once
#include "Editor/Core/Service/IService.h"
#include <string>
#include <string_view>
#include <vector>

namespace DM
{
	/// <summary>
	/// 资产目录扫描器(服务)：检测资产目录是否发生变化，并与 AssetMetaDatabase 对比
	/// 发现尚未导入的新资源。
	/// </summary>
	class AssetScanner : public IService
	{
	public:
		AssetScanner() = default;

		struct NewAsset
		{
			std::string SourceFilePath; // 源文件完整路径
			std::string FileName;       // 显示用文件名
		};

		/// <summary>
		/// 扫描资产目录：目录相对上次记录未变化时返回空；
		/// 变化时与数据库对比，返回新发现的可导入资源列表
		/// </summary>
		std::vector<NewAsset> Scan();

		/// <summary>
		/// 导入单个源文件(生成 .dasset 资产包并注册到数据库)
		/// </summary>
		bool Import(const std::string& sourceFilePath);

		/// <summary>
		/// 检测源文件自上次导入后是否发生修改(时间戳/内容哈希双校验)。
		/// 属于编辑器侧"是否需重导入"的决策，供扫描/重导入流程使用。
		/// </summary>
		bool IsSourceFileModified(std::string_view sourceFilePath);

	private:
		/// <summary>
		/// 目录级指纹：仅遍历目录节点，记录每个目录的相对路径与修改时间。
		/// 注：不能只看根目录mtime——Windows上目录mtime只随直接子项变化，深层子目录中新增文件不会向上传播。
		/// </summary>
		std::string ComputeDirFingerprint();

		/// <summary>
		/// 数据库文件状态戳(存在性+mtime+大小)：数据库被删除/重建时即使目录指纹未变
		/// (数据库文件位于资产根目录，不在目录指纹内)也必须强制重新对比。
		/// </summary>
		std::string ComputeDatabaseStamp();
		std::string LoadCachedFingerprint();
		std::string LoadCachedDatabaseStamp();
		void SaveFingerprint(const std::string& fingerprint);
	};
}
