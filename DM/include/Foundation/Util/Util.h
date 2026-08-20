#pragma once
#include<string.h>
namespace DM
{
	class DM_API Util
	{
	public:
		static std::string				WStringToString_U8(const std::wstring& str);
		static std::wstring				StringToWString_U8(const std::string& str);

		static bool						SkipUtf8Bom(std::ifstream& file);
		static bool						HasUtf8Bom(std::ifstream& file);
		static bool						HasUtf8Bom(std::string& content);
		static void						RemoveUtf8Bom(std::string& content);

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


