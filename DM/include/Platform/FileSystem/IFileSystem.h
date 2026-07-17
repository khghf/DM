#pragma once
#include <string>
#include <vector>
#include <Core/Core.h>

namespace DM
{
	// 文件系统抽象接口（L1 Platform 层）。
	// 上层通过接口读写文件、遍历目录、解析路径，不依赖具体 OS API。
	class DM_API IFileSystem
	{
	public:
		virtual ~IFileSystem() = default;

		virtual bool ReadFile(const std::string& path, std::string& outContent) = 0;
		virtual bool WriteFile(const std::string& path, const std::string& content) = 0;
		virtual bool Exists(const std::string& path) = 0;
		virtual bool IsRegularFile(const std::string& path) = 0;

		// 列出目录下所有文件（不含子目录递归由调用方决定是否递归）。
		virtual std::vector<std::string> ListFiles(const std::string& directory) = 0;

		// 路径解析辅助。
		virtual std::string GetParentPath(const std::string& path) = 0;
		virtual std::string GetFileName(const std::string& path) = 0;
		virtual std::string GetExtension(const std::string& path) = 0;
		virtual std::string Canonicalize(const std::string& path) = 0;
	};
}
