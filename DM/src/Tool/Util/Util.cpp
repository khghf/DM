#include "DMPCH.h"
#include "Tool/Util/Util.h"
#include<codecvt>
#include<locale>
#include<fstream>
#include<sstream>
#include<filesystem>
#include<Core/Core.h>
#include<regex>
#include<Windows.h>
namespace fs = std::filesystem;
namespace DM
{
#ifdef _WIN32
	std::string Util::WStringToString_U8(const std::wstring& str)
	{
		if (str.empty())
		{
			return {};
		}
		int utf8_buf_size = WideCharToMultiByte(
			CP_UTF8,               // 转换编码格式为UTF-8
			0,                     // 无特殊转换标志
			str.c_str(),           // 源宽字符串
			static_cast<int>(str.size()),  // 源字符串长度（字符数）
			nullptr,               // 临时为NULL，仅获取所需缓冲区大小
			0,                     // 缓冲区大小为0，仅计算所需长度
			nullptr,               // 不使用默认替换字符
			nullptr                // 不检测默认替换字符使用情况
		);

		// 检查API调用是否失败
		if (utf8_buf_size == 0)
		{
			DWORD error_code = GetLastError();
			throw std::runtime_error("WideCharToMultiByte failed, error code: " + std::to_string(error_code));
		}
		std::string utf8_str(utf8_buf_size, 0);
		int result = WideCharToMultiByte(
			CP_UTF8,
			0,
			str.c_str(),
			static_cast<int>(str.size()),
			&utf8_str[0],          // 目标缓冲区
			utf8_buf_size,
			nullptr,
			nullptr
		);

		if (result == 0)
		{
			DWORD error_code = GetLastError();
			throw std::runtime_error("WideCharToMultiByte conversion failed, error code: " + std::to_string(error_code));
		}

		return utf8_str;
	}
	std::wstring Util::StringToWString_U8(const std::string& str)
	{
		if (str.empty())
		{
			return {};
		}

		int wide_buf_size = MultiByteToWideChar(
			CP_UTF8,               // 源编码为UTF-8
			0,                     // 无特殊转换标志
			str.c_str(),           // 源多字节字符串
			static_cast<int>(str.size()),  // 源字符串长度（字节数）
			nullptr,               // 临时为NULL，仅获取所需缓冲区大小
			0                      // 缓冲区大小为0，仅计算所需长度
		);

		if (wide_buf_size == 0)
		{
			DWORD error_code = GetLastError();
			throw std::runtime_error("MultiByteToWideChar failed, error code: " + std::to_string(error_code));
		}

		std::wstring wide_str(wide_buf_size, 0);
		int result = MultiByteToWideChar(
			CP_UTF8,
			0,
			str.c_str(),
			static_cast<int>(str.size()),
			&wide_str[0],          // 目标缓冲区
			wide_buf_size
		);

		if (result == 0)
		{
			DWORD error_code = GetLastError();
			throw std::runtime_error("MultiByteToWideChar conversion failed, error code: " + std::to_string(error_code));
		}

		return wide_str;
	}
#endif // _WIN32
	std::vector<std::string> Util::GetAllFilePath(const std::string& FloderPath, bool GetSubFloderFile)
	{
		std::vector<std::string>FilePaths;
		fs::path Path = FloderPath;
		if (!fs::is_directory(Path) || !fs::exists(Path))
		{
			LOG_CORE_ERROR("passed a useless floder path:{}", fs::absolute(Path).string());
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
	std::string Util::ReadFile(const std::string& Path)
	{
		std::ifstream textFile(Path, std::ios::in | std::ios::binary);
		if (textFile)
		{
			std::string context = "";
			std::stringstream ss;
			ss << textFile.rdbuf();
			textFile.close();
			context = ss.str();
			RemoveUtf8Bom(context);
			return context;
		}
		else
		{
			LOG_CORE_ERROR("Could not open file:{}", fs::absolute(Path).string());
		}
		return {};
	}
	std::string Util::GetSuffix(const std::string& str)
	{
		std::string ret = str;
		size_t index = ret.find_last_of(".");
		if (index + 1 != std::string::npos)
		{
			ret = ret.substr(index + 1);
		}
		return ret;
	}

	std::string Util::GetFileName(const std::string& filePath, bool bWithSuffix)
	{
		std::string path = filePath;
		size_t index = -1;
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
		size_t index = ret.find_last_of(ch);
		if (index != std::string::npos)
		{
			ret = ret.substr(index + 1);
		}
		return ret;
	}

	std::string Util::NormalizeFilePath(const std::string path)
	{
		fs::path p(path);
		return fs::canonical(p).string();
		/*std::string ret = path;
		ret = std::regex_replace(ret, std::regex(R"(\\\\)", std::regex::nosubs), " / ");
		ret = std::regex_replace(ret, std::regex(R"(\\)", std::regex::nosubs), " / ");
		return ret;*/
	}

	bool Util::SkipUtf8Bom(std::ifstream& file)
	{
		const std::vector<unsigned char> UTF8_BOM = { 0xEF, 0xBB, 0xBF };
		if (!file.is_open() || !file.good()) {
			DM_CORE_ASSERT(false, "{}", "ERROR:ifstream file open failed");
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

	bool Util::HasUtf8Bom(std::ifstream& file)
	{
		if (!file.is_open() || !file.good()) {
			DM_CORE_ASSERT(false, "{}", "ERROR:ifstream file open failed");
			return false;
		}
		const std::vector<unsigned char> UTF8_BOM = { 0xEF, 0xBB, 0xBF };
		file.seekg(0, std::ios::beg);
		std::vector<unsigned char> bomCandidate(UTF8_BOM.size());
		file.read(reinterpret_cast<char*>(bomCandidate.data()), bomCandidate.size());
		bool hasUtf8Bom = (bomCandidate == UTF8_BOM);
		return hasUtf8Bom;
	}

	bool Util::HasUtf8Bom(std::string& content)
	{
		const char utf8_bom[] = "\xEF\xBB\xBF";
		return content.size() >= 3 && content.substr(0, 3) == utf8_bom;
	}

	void Util::RemoveUtf8Bom(std::string& content)
	{
		const char utf8_bom[] = "\xEF\xBB\xBF";
		if (content.size() >= 3 && content.substr(0, 3) == utf8_bom) {
			content.erase(0, 3); // 去掉BOM头
		}
	}

	bool Util::CretaFloderIfNotExist(const std::string folderPath)
	{
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
		size_t index = ret.find_last_of(".");
		if (index != std::string::npos)
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

	bool Util::HasSuffix(const std::string& source, const std::string& suffix)
	{
		return suffix == GetSuffix(source);
	}

}