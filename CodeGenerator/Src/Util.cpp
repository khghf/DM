#include"Util.h"
#include<filesystem>
#include<regex>
#include<fstream>
#include<sstream>
#include<stdexcept>
#include<iostream>
namespace reflect
{
	namespace fs = std::filesystem;

	std::string Util::DisplayName(CXCursor cursor)
	{
		std::string ret;
		CXString str = clang_getCursorDisplayName(cursor);
		ret = clang_getCString(str);
		clang_disposeString(str);
		return ret;
	}

	std::string Util::CursorSpelling(CXCursor cursor)
	{
		std::string ret;
		CXString str = clang_getCursorSpelling(cursor);
		ret = clang_getCString(str);
		clang_disposeString(str);
		return ret;
	}

	std::string Util::CursorTypeSpelling(CXCursor cursor)
	{
		std::string ret;
		CXType type = clang_getCursorType(cursor);
		return TypeSpelling(type);
	}



	std::string Util::TypeSpelling(CXType type)
	{
		std::string ret;
		CXString str = clang_getTypeSpelling(type);
		ret = clang_getCString(str);
		clang_disposeString(str);
		return ret;
	}

	bool Util::IsTargetHeaderCursor(CXCursor cursor, const std::string& targetHeaderName)
	{
		// 1. 获取光标对应的源文件位置
		CXSourceLocation cursorLoc = clang_getCursorLocation(cursor);
		if (clang_Location_isInSystemHeader(cursorLoc)) {
			// 直接跳过系统头文件（如<iostream>）的光标
			return false;
		}

		// 2. 提取光标所属的文件对象
		CXFile cursorFile = nullptr;
		clang_getSpellingLocation(cursorLoc, &cursorFile, nullptr, nullptr, nullptr);
		if (cursorFile == nullptr) {
			return false;
		}
		// 3. 获取光标所属文件的完整路径
		CXString str= clang_getFileName(cursorFile);
		const char* cursorFileNameCStr = clang_getCString(str);
		if (cursorFileNameCStr == nullptr) {
			return false;
		}
		std::string cursorFileName = GetFileName(cursorFileNameCStr,true);
		//std::cout << cursorFileName << "11111"<<std::endl;
		clang_disposeString(str);
		// 5. 判定：仅当光标文件路径与目标文件路径完全一致时，返回true
		return (cursorFileName == targetHeaderName);
	}

	bool Util::IsCursorInRange(CXCursor cursor, CXCursor targetNode)
	{
		CXSourceRange targetRange = clang_getCursorExtent(targetNode);
		CXSourceRange cursorRange = clang_getCursorExtent(cursor);
		return clang_equalRanges(cursorRange, targetRange);
	}
	void Util::GetCursorLocation(CXCursor cursor, uint32_t* out)
	{
		CXSourceLocation location=clang_getCursorLocation(cursor);
		unsigned int line = 0, col = 0;
		clang_getPresumedLocation(location, nullptr, &line, &col);
		out[0] = line;
		out[1] = col;
	}

	void Util::GetCursorLocation(CXCursor cursor, uint32_t* line, uint32_t* col)
	{
		CXSourceLocation location = clang_getCursorLocation(cursor);
		clang_getPresumedLocation(location, nullptr, line, col);
	}

	uint32_t Util::GetIncludeNumInStr(const std::string& str)
	{
		uint32_t res = 0;
		std::regex re(R"(#include[\s]*(<.*>|".*"))");
		std::sregex_token_iterator it(str.begin(), str.end(), re);
		std::sregex_token_iterator end;
		while (it!=end)
		{
			if (!it->str().empty()) ++res;
			++it;
		}
		return res;
	}

	uint32_t Util::GetIncludeNumInFile(const std::string& filePath)
	{
		std::ifstream file(filePath);
		uint32_t res = 0;
		if (!file.is_open())
		{
			std::cerr << filePath + "is invalid!\n";
			throw std::runtime_error(filePath + "is invalid!");
			return res;
		}
		std::stringstream s;
		SkipUtf8Bom(file);
		s << file.rdbuf();
		const std::string content = s.str();
		return GetIncludeNumInStr(content);
	}

	uint32_t Util::GetLastIncludeInFile(const std::string& filePath)
	{
		std::ifstream file(filePath);
		uint32_t pos = 0;
		if (!file.is_open())
		{
			std::cerr << filePath + "is invalid!\n";
			throw std::runtime_error(filePath + "is invalid!");
			return pos;
		}
		std::stringstream s;
		SkipUtf8Bom(file);
		uint32_t skiped = 0;
		std::string line;
		while (std::getline(file,line))
		{
			if (std::regex_match(line, std::regex(R"(#include[\s]*(<.*>|".*"))", std::regex::nosubs)))
			{
				pos += skiped+1;
				skiped = 0;
			}
			else
			{
				++skiped;
			}
		}
		return pos;
	}

	std::string Util::GetFileSuffix(const std::string& path)
	{
		std::string ret = path;
		int index = ret.find_last_of(".");
		if (index + 1 != std::string::npos)
		{
			ret = ret.substr(index + 1);
		}
		return ret;
	}

	std::string Util::GetFileName(const std::string& filePath, bool bWithSuffix)
	{
		std::string path = filePath;
		int index = -1;
		index = path.find_last_of("/\\");
		if (index + 1 != std::string::npos)
		{
			path = path.substr(index + 1);
		}
		if (!bWithSuffix)
		{
			index = path.find_last_of(".");
			if (index != std::string::npos)
			{
				path = path.substr(0, index);
			}
		}
		return path;
	}

	std::string Util::GetStrBehindLastChar(const std::string source, const char* ch)
	{
		std::string ret = source;
		int index = ret.find_last_of(ch);
		if (index != std::string::npos)
		{
			ret = ret.substr(index + 1);
		}
		return ret;
	}

	bool Util::CretaFloderIfNotExist(const std::string folderPath)
	{
		if (folderPath.empty())return false;
		fs::path path(folderPath);
		if (fs::exists(path) && fs::is_directory(path))return true;
		return fs::create_directories(path);
	}

	bool Util::IsFileExist(const std::string filePath)
	{
		fs::path path(filePath);
		if (fs::exists(filePath) && fs::is_regular_file(path))return true;
		return false;
	}

	

	bool Util::IsDirectoryExist(const std::string filePath)
	{
		fs::path path(filePath);
		if (fs::exists(filePath) && fs::is_directory(path))return true;
		return false;
	}

	std::string Util::ModifySuffix(const std::string& source, const std::string& suffix)
	{
		std::string ret = source;
		int index = ret.find_last_of(".");
		if (index!= std::string::npos)
		{
			ret = ret.substr(0, index + 1);
			ret += suffix;
		}
		else
		{
			ret += ("." + suffix);
		}
		return ret;
	}

	std::vector<std::string> Util::GetAllFilePath(const std::string& FloderPath, bool GetSubFloderFile /*= false*/)
	{
		std::vector<std::string>FilePaths;
		fs::path Path = FloderPath;
		if (!fs::is_directory(Path) || !fs::exists(Path))
		{
			FilePaths.push_back(FloderPath);
			return FilePaths;
		}
		for (const auto& File : fs::directory_iterator(Path))
		{
			if (File.is_regular_file())
			{
				FilePaths.push_back(File.path().string());
			}
			else
			{
				auto SubFiles = GetAllFilePath(File.path().string(), GetSubFloderFile);
				FilePaths.insert(FilePaths.end(), SubFiles.begin(), SubFiles.end());
			}
		}
		return FilePaths;
	}

	std::vector<std::string> Util::GetAllFilePath(const std::vector<std::string>& FloderPaths, bool GetSubFloderFile /*= false*/)
	{
		std::vector<std::string>res;
		for (const auto& path : FloderPaths)
		{
			std::vector<std::string>p=GetAllFilePath(path, GetSubFloderFile);
			res.insert(res.end(), p.begin(), p.end());
		}
		return res;
	}

	std::vector<std::string> Util::GetAllFilePath(const std::vector<std::string>& FloderPaths, bool GetSubFloderFile /*= false*/, std::set<std::string>exculdPaths /*= {}*/)
	{
		std::vector<std::string>res;
		for (const auto& path : FloderPaths)
		{
			if (exculdPaths.find(path) != exculdPaths.end())continue;
			std::vector<std::string>p = GetAllFilePath(path, GetSubFloderFile);
			res.insert(res.end(), p.begin(), p.end());
		}
		return res;
	}

	std::string Util::NormalizeFilePath(const std::string path)
	{
		std::string ret = path;
		ret = std::regex_replace(ret, std::regex(R"(\\\\)", std::regex::nosubs), " / ");
		ret = std::regex_replace(ret, std::regex(R"(\\)", std::regex::nosubs), " / ");
		return ret;
	}

	std::string Util::AbsPath(const std::string path)
	{
		return fs::absolute(path).string();
	}

	bool Util::SkipUtf8Bom(std::ifstream& file)
	{
		const std::vector<unsigned char> UTF8_BOM = { 0xEF, 0xBB, 0xBF };
		if (!file.is_open() || !file.good()) {
			std::cerr << "错误：文件未正常打开，无法检查BOM头" << std::endl;
			return false;
		}
		file.seekg(0, std::ios::beg);
		std::vector<unsigned char> bomCandidate(UTF8_BOM.size());
		file.read(reinterpret_cast<char*>(bomCandidate.data()), bomCandidate.size());
		bool hasUtf8Bom = (bomCandidate == UTF8_BOM);
		if (hasUtf8Bom) {
			return true;
		}
		else {
			file.seekg(0, std::ios::beg);
			return true;
		}
	}

	void Util::RemoveUtf8Bom(std::string& content)
	{
		const char utf8_bom[] = "\xEF\xBB\xBF";
		if (content.size() >= 3 && content.substr(0, 3) == utf8_bom) {
			content.erase(0, 3); // 去掉BOM头
		}
	}

}