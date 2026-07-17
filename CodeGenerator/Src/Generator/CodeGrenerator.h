#pragma once
#include<string>
namespace reflect
{
	class CodeGrenerator
	{
		friend class CodeParser;
	public:
		static void GenerateCode();
		static std::string FileStorageFloder;
	private:
		static void Generate_h();
		static void Generate_cpp();
		static const std::string include_angled(const std ::string &path);
		static const std::string include_quotes(const std::string& path);
		//自动添加.gen.h头文件包含到对应的.h文件,文件中没有该头文件时反回true,有返回false;
		static void InsertIncludeToFile(const std::string& filePath, const std ::string&includestr);
		static void ClearInclude(const std::string& filePath, const std::string& includestr);
	};
}


