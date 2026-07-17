#pragma once
#include<string>
#include<clang-c/Index.h>
#include<vector>
#include<set>
namespace reflect
{
	class Util
	{
	public:
		static std::string	DisplayName(CXCursor cursor);
		static std::string	CursorSpelling(CXCursor cursor);
		static std::string	CursorTypeSpelling(CXCursor cursor);
		static std::string	TypeSpelling(CXType type);
		static bool			IsTargetHeaderCursor(CXCursor cursor, const std::string& targetHeaderPath);
		static bool			IsCursorInRange(CXCursor cursor, CXCursor targetNode);
		//返回行号列号
		static void						GetCursorLocation(CXCursor cursor, uint32_t* out);
		static void						GetCursorLocation(CXCursor cursor, uint32_t* line, uint32_t* col);
		static uint32_t					GetIncludeNumInStr(const std::string& str);
		static uint32_t					GetIncludeNumInFile(const std::string& filePath);
		static uint32_t					GetLastIncludeInFile(const std::string& filePath);
		static std::string				GetFileSuffix(const std::string& path);
		static std::string				GetFileName(const std::string& filePath, bool bWithSuffix);
		static std::string				GetStrBehindLastChar(const std::string source, const char* ch);
		static std::vector<std::string>	GetAllFilePath(const std::string& FloderPath, bool GetSubFloderFile = false);
		static std::vector<std::string>	GetAllFilePath(const std::vector<std::string>& FloderPaths, bool GetSubFloderFile = false, std::set<std::string>exculdPaths = {});
		static std::vector<std::string>	GetAllFilePath(const std::vector<std::string>&FloderPaths, bool GetSubFloderFile = false);

		static std::string	NormalizeFilePath(const std::string path);
		static std::string	AbsPath(const std::string path);
		static bool			SkipUtf8Bom(std::ifstream& file);
		static void			RemoveUtf8Bom(std::string& content);
		static bool			CretaFloderIfNotExist(const std::string folderPath);
		static bool			IsFileExist(const std::string filePath);
		static bool			IsDirectoryExist(const std::string filePath);
		static std::string	ModifySuffix(const std::string& source, const std::string& suffix);
	};
}