#pragma once
#include<string.h>
namespace DM
{
	class DM_API Util
	{
	public:
		static std::string			WStringToString_U8(const std::wstring& str);
		static std::wstring			StringToWString_U8(const std::string& str);
		static std::string			ReadFile(const std::string& Path);


		static std::vector<std::string>	GetAllFilePath(const std::string& FloderPath, bool GetSubFloderFile = false);
		static std::string			GetSuffix(const std::string& str);
		static std::string			GetFileName(const std::string& filePath, bool bWithSuffix);
		static std::string			GetStrBehindLastChar(const std::string source, const char* ch);


		static std::string			NormalizeFilePath(const std::string path);
		static bool					SkipUtf8Bom(std::ifstream& file);
		static bool					HasUtf8Bom(std::ifstream& file);
		static bool					HasUtf8Bom(std::string& content);
		static void					RemoveUtf8Bom(std::string& content);
		static bool					CretaFloderIfNotExist(const std::string folderPath);
		static bool					IsFileExist(const std::string filePath);
		static bool					IsDirectoryExist(const std::string filePath);
		static std::string			ModifySuffix(const std::string& source, const std::string& suffix);
		static bool					HasSuffix(const std::string& source, const std::string& suffix);
	};
	struct DM_API Is_Pointer
	{
		template<typename Ty>
		constexpr static bool Val(Ty&& obj)
		{
			return std::is_pointer_v<std::remove_reference_t<Ty>>;
		}
	};


}


