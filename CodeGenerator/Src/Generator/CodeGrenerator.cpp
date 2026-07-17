#include "CodeGrenerator.h"
#include"../Meta/MetaInfo.h"
#include<string>
#include<string_view>
#include"../Util.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include<regex>
#include<filesystem>
namespace reflect
{
	namespace fs = std::filesystem;
	std::string CodeGrenerator::FileStorageFloder = "";
	std::string_view GeneratedCppIncludes = "Reflect/GeneratedCppIncludes.h";
	std::string_view GeneratedHppIncludes = "Reflect/GeneratedHppIncludes.h";
	MetaFile* Info = nullptr;
	void CodeGrenerator::GenerateCode()
	{
		Util::CretaFloderIfNotExist(FileStorageFloder);
		for (auto& it : MetaInfo::GetMetaRegistry())
		{
			Info = &it.second;
			//解析文件中有CLASS()宏则为true,用来避免生成不必要的.gen文件
			if (!Info->bHasReflectMark)
			{
				const std::string fileName = Util::GetFileName(Info->Path, false) + ".gen.h";
				std::string includePath = include_quotes(fileName);//include_angled会在末尾加上\n
				ClearInclude(Util::ModifySuffix(Info->Path, "h"), includePath.substr(0, includePath.size() - 1));
				continue;
			}
			Generate_h();
			Generate_cpp();
		}
	}
	void CodeGrenerator::Generate_h()
	{
		const std::string fileName = Util::GetFileName(Info->Path, false)+".gen.h";

		std::string savePath = FileStorageFloder.data();
		savePath = savePath + "/"+ fileName;
		std::string includePath = include_quotes(fileName);//include_angled会在末尾加上\n
		InsertIncludeToFile(Util::ModifySuffix(Info->Path, "h"), includePath.substr(0, includePath.size()-1));
		std::ofstream out(savePath, std::ios_base::trunc);
		std::stringstream content;
		content << "#pragma once\n";
		//content << include_angled(GeneratedHppIncludes.data());
		for (const auto& metaClass : Info->Classes)
		{
			int nameSpaceCount = metaClass.NameSpaces.size();
			std::string nameSpace = "";
			if (nameSpaceCount > 0)
			{
				for (int i = nameSpaceCount - 1; i >= 0; --i)
				{
					if (i != 0)
					{
						nameSpace += metaClass.NameSpaces[i] + "::";
					}
					else
					{
						nameSpace += metaClass.NameSpaces[i];
					}
				}
			}
			const std::string& className = metaClass.Name;
			if (metaClass.ReflectBodyLine == 0)
			{
				std::string estr = "ERROR:Marco REFLECT_BODY must be used in reflection class " + className;
				std::cerr << estr << std::endl;
				throw std::runtime_error(estr.c_str());
			}
			else
			{
				//REFLECT_BODY()----------
				content << "#undef REFLECT_BODY_" + std::to_string(metaClass.ReflectBodyLine) + "\n";
				content << "#define REFLECT_BODY_" + std::to_string(metaClass.ReflectBodyLine) + "()\\\n";
				content << "typedef " + className + " ThisClass;\\\n";
				content << "public:\\\n";
				content << "static ::mirror::TypeId GetTypeId();\\\n";//返回类自身的TypeId*
				content << "private:\n";
				//REFLECT_BODY()----------
			}
		}
		out << content.str();
	}


	void CodeGrenerator::Generate_cpp()
	{
		const std::string fileName = Util::GetFileName(Info->Path, false) + ".gen.cpp";
		std::string savePath = FileStorageFloder.data();
		savePath = savePath + "/" + fileName;

		std::ofstream out(savePath, std::ios_base::trunc);
		std::stringstream content;
		//content << include_angled(GeneratedCppIncludes.data());

		content << include_quotes( Util::ModifySuffix(Util::AbsPath(Info->Path),"h"));
		for (const auto& metaClass : Info->Classes)
		{
			const std::string& className = metaClass.Name;
			int nameSpaceCount = metaClass.NameSpaces.size();
			std::string nameSpace = "";
			if (nameSpaceCount > 0)
			{
				for (int i = nameSpaceCount - 1; i >= 0; --i)
				{
					if (i != 0)
					{
						nameSpace += metaClass.NameSpaces[i] + "::";
					}
					else
					{
						nameSpace += metaClass.NameSpaces[i];
					}
				}
			}

			content << "REGISTER_TYPE(" + nameSpace+"::" + className + ")\n";
			for (const auto& metafun : metaClass.Functions)
			{
				if (metafun.IsPublic)content << "REGISTER_MEMBER_FUNCTION(" + nameSpace + "::" + className + ")\n";
				else				 content << "REGISTER_PRIVATE_MEMBER_FUNCTION(" + nameSpace + "::" + className + ")\n";
			}
			for (const auto& metaField : metaClass.Fields)
			{
				if (metaField.IsPublic)	content << "REGISTER_MEMBER(" + nameSpace + "::" + className + ","+metaField.Name+")\n";
				else					content << "REGISTER_PRIVATE_MEMBER(" + nameSpace + "::" + className + "," + metaField.Name + ")\n";
			}
			//***************************************************************为类生成的成员函数

			content << "::mirror::TypeId " + nameSpace + "::" + className + "::GetTypeId()\n";//返回类自身的TypeId*
			content << "{\n";
			content << "\treturn ::mirror::GetTypeId<" + nameSpace + "::" + className + ">(); \n";
			content << "}\n";
		
		}
		out << content.str();
	}

	const std::string CodeGrenerator::include_angled(const std::string& path)
	{
		return "#include<" + path + ">\n";
	}

	const std::string CodeGrenerator::include_quotes(const std::string& path)
	{
		return "#include\"" + path + "\"\n";
	}

	void CodeGrenerator::InsertIncludeToFile(const std::string& filePath, const std::string& includestr)
	{
		if (!fs::is_regular_file(filePath))return ;
		std::ifstream file(filePath, std::ios::in);
		//先去掉可能存在的想要插入的头文件
		std::stringstream steam;
		steam << file.rdbuf();
		std::string content = steam.str();
		Util::RemoveUtf8Bom(content);
		const std::string includeStrModified = includestr + "(\n|\r)*";
		std::regex re(includeStrModified, std::regex::nosubs);
		uint32_t bExisit = 0;
		if (std::regex_search(content, re))
		{
			content = std::regex_replace(content, re, "");
			file.close();
			std::ofstream out(filePath, std::ios_base::trunc);
			out << content;
			out.close();
			file.open(filePath);
			bExisit = 1;
		}
		else
		{
			for (int i = 0; i < Info->Classes.size(); ++i)
			{
				++Info->Classes[i].ReflectBodyLine;
			}
		}
		Util::SkipUtf8Bom(file);
		std::vector<std::string> fileLines;
		std::string line;
		uint32_t CurLine = 0;
		bool bPushed = false;
		while (std::getline(file, line)) {
			++CurLine;
			if (CurLine > (Info->LastIncludeLine-bExisit)&&!bPushed)
			{
				bPushed = true;
				fileLines.emplace_back(includestr+"\n");
			}
			fileLines.emplace_back(std::move(line) + "\n");
		}
		file.close();
		std::ofstream out(filePath, std::ios_base::trunc);
		for (const auto& str : fileLines)
		{
			out << str;
		}
		out.close();
	}

	void CodeGrenerator::ClearInclude(const std::string& filePath, const std::string& includestr)
	{
		if (!fs::is_regular_file(filePath))return;
		std::ifstream in(filePath);
		std::string content;
		Util::SkipUtf8Bom(in);
		std::stringstream stream;
		stream << in.rdbuf();
		in.close();
		content =stream.str();
		const std::string includeStrModified = includestr + "(\n|\r)*";
		std::regex re(includeStrModified, std::regex::nosubs);
		content=std::regex_replace(content, re, "");
		std::ofstream out(filePath);
		out << content;
	}
}