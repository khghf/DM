#pragma once
#include <string>
#include <vector>
#include <Core/Core.h>

namespace DM
{
    class DM_API FileSystem
    {
    public:
        /// <summary>
        /// 读取文件全部内容到字符串(文本模式)
        /// </summary>
        static bool ReadFile(const std::string& path, std::string& outContent);

        /// <summary>
        /// 读取文件全部内容到字节数组(二进制模式)
        /// </summary>
        static bool ReadFileBinary(const std::string& path, std::vector<uint8_t>& outContent);

        /// <summary>
        /// 写入字符串到文件(文本模式)
        /// </summary>
        static bool WriteFile(const std::string& path, const std::string& content);

        /// <summary>
        /// 写入字节数组到文件(二进制模式)
        /// </summary>
        static bool WriteFileBinary(const std::string& path, const std::vector<uint8_t>& content);

        /// <summary>
        /// 追加字符串到文件末尾
        /// </summary>
        static bool AppendFile(const std::string& path, const std::string& content);

        // ============ 文件/目录信息 ============

        /// <summary>
        /// 检查路径是否存在
        /// </summary>
        static bool Exists(const std::string& path);

        /// <summary>
        /// 检查是否是普通文件
        /// </summary>
        static bool IsRegularFile(const std::string& path);

        /// <summary>
        /// 检查是否是目录
        /// </summary>
        static bool IsDirectory(const std::string& path);

        /// <summary>
        /// 检查是否是符号链接
        /// </summary>
        static bool IsSymlink(const std::string& path);

        /// <summary>
        /// 检查是否为空文件或空目录
        /// </summary>
        static bool IsEmpty(const std::string& path);

        /// <summary>
        /// 获取文件大小(字节)
        /// </summary>
        static uint64_t GetFileSize(const std::string& path);

        /// <summary>
        /// 获取文件最后修改时间(时间戳)
        /// </summary>
        static uint64_t GetLastWriteTime(const std::string& path);

        /// <summary>
        /// 获取文件最后修改时间(字符串格式)
        /// </summary>
        static std::string GetLastWriteTimeString(const std::string& path, const std::string& format = "%Y-%m-%d %H:%M:%S");

        // ============ 路径操作 ============

        /// <summary>
        /// 获取父目录路径
        /// </summary>
        static std::string GetParentPath(const std::string& path);

        /// <summary>
        /// 获取文件名(含扩展名)
        /// </summary>
        static std::string GetFileName(const std::string& path);

        /// <summary>
        /// 获取文件名(不含扩展名)
        /// </summary>
        static std::string GetFileNameWithoutExtension(const std::string& path);

        /// <summary>
        /// 获取扩展名(含点号)
        /// </summary>
        static std::string GetExtension(const std::string& path);

        /// <summary>
        /// 获取扩展名(不含点号)
        /// </summary>
        static std::string GetExtensionWithoutDot(const std::string& path);

        /// <summary>
        /// 修改文件名(只修改路径字符串，不实际重命名文件)
        /// </summary>
        static void ChangeFileName(std::string& path, const std::string& newName);

        /// <summary>
        /// 在文件名后追加字符串(如 "texture" -> "texture_v2")
        /// </summary>
        static void AppendStrAfterFileName(std::string& path, const std::string& str);

        /// <summary>
        /// 在扩展名后追加字符串(如 ".png" -> ".png.compressed")
        /// </summary>
        static void AppendStrAfterExtension(std::string& path, const std::string& str);

        /// <summary>
        /// 替换扩展名
        /// </summary>
        static std::string ReplaceExtension(const std::string& path, const std::string& newExtension);

        /// <summary>
        /// 拼接路径
        /// </summary>
        static std::string JoinPath(const std::string& base, const std::string& relative);

        /// <summary>
        /// 规范化路径(统一分隔符、解析 . 和 ..)
        /// </summary>
        static std::string NormalizePath(const std::string& path);

        /// <summary>
        /// 获取绝对路径
        /// </summary>
        static std::string GetAbsolutePath(const std::string& path);

        /// <summary>
        /// 获取规范化的绝对路径(解析符号链接)
        /// </summary>
        static std::string Canonicalize(const std::string& path);

        /// <summary>
        /// 检查是否是绝对路径
        /// </summary>
        static bool IsAbsolutePath(const std::string& path);

        /// <summary>
        /// 获取相对路径
        /// </summary>
        static std::string GetRelativePath(const std::string& path, const std::string& base);

        // ============ 目录操作 ============

        /// <summary>
        /// 列出目录中的文件(不递归)
        /// </summary>
        static std::vector<std::string> ListFiles(const std::string& directory, bool bRecursive = false);

        /// <summary>
        /// 列出目录中的所有文件和子目录(不递归)
        /// </summary>
        static std::vector<std::string> ListEntries(const std::string& directory, bool bRecursive = false);

        /// <summary>
        /// 列出目录中的所有子目录(不递归)
        /// </summary>
        static std::vector<std::string> ListDirectories(const std::string& directory, bool bRecursive = false);

        /// <summary>
        /// 列出匹配模式的文件
        /// </summary>
        static std::vector<std::string> ListFilesWithPattern(const std::string& directory, const std::string& pattern, bool bRecursive = false);

        /// <summary>
        /// 创建目录(自动创建父目录)
        /// </summary>
        static bool CreateDirectories(const std::string& path);

        /// <summary>
        /// 创建目录(仅创建最后一级)
        /// </summary>
        static bool CreateDirectorySingle(const std::string& path);

        /// <summary>
        /// 删除空目录
        /// </summary>
        static bool DeleteDirectory(const std::string& path);

        /// <summary>
        /// 递归删除目录及其所有内容
        /// </summary>
        static bool DeleteDirectoryRecursive(const std::string& path);

        /// <summary>
        /// 清空目录(删除所有内容但保留目录本身)
        /// </summary>
        static bool ClearDirectory(const std::string& path);

        /// <summary>
        /// 复制目录(不递归)
        /// </summary>
        static bool CopyDirectory(const std::string& source, const std::string& destination, bool bRecursive = false);

        // ============ 文件操作 ============

        /// <summary>
        /// 删除文件
        /// </summary>
        static bool DeleteFile_(const std::string& path);

        /// <summary>
        /// 复制文件
        /// </summary>
        static bool CopyFile_(const std::string& source, const std::string& destination, bool bOverwrite = false);

        /// <summary>
        /// 移动/重命名文件或目录
        /// </summary>
        static bool Move(const std::string& source, const std::string& destination);

        /// <summary>
        /// 重命名文件或目录(同 Move)
        /// </summary>
        static bool Rename(const std::string& source, const std::string& destination);

        // ============ 系统路径 ============

        /// <summary>
        /// 获取当前工作目录
        /// </summary>
        static std::string GetCurrentDirectory_();

        /// <summary>
        /// 设置当前工作目录
        /// </summary>
        static bool SetCurrentDirectory_(const std::string& path);

        /// <summary>
        /// 获取临时目录
        /// </summary>
        static std::string GetTempDirectory();

        /// <summary>
        /// 获取临时文件名
        /// </summary>
        static std::string GetTempFileName_(const std::string& prefix = "", const std::string& extension = ".tmp");

        /// <summary>
        /// 获取用户主目录
        /// </summary>
        static std::string GetHomeDirectory();

        /// <summary>
        /// 获取可执行文件所在目录
        /// </summary>
        static std::string GetExecutableDirectory();

        // ============ 其他实用功能 ============

        /// <summary>
        /// 生成唯一的文件名
        /// </summary>
        static std::string GenerateUniqueFileName(const std::string& directory, const std::string& baseName, const std::string& extension);

        /// <summary>
        /// 确保目录存在(不存在则创建)
        /// </summary>
        static bool EnsureDirectoryExists(const std::string& path);

        /// <summary>
        /// 确保文件所在目录存在
        /// </summary>
        static bool EnsureFileDirectoryExists(const std::string& filePath);

        /// <summary>
        /// 获取文件哈希(MD5)
        /// </summary>
        static std::string GetFileMD5(const std::string& path);

        /// <summary>
        /// 比较两个文件内容是否相同
        /// </summary>
        static bool AreFilesEqual(const std::string& path1, const std::string& path2);
    };
}