#include"MetaInfo.h"
#include<unordered_map>
#include<iostream>
namespace reflect
{
	std::unordered_map<std::string, MetaFile>MetaRegistry;
	MetaFile* CurFile = nullptr;
	MetaFile* MetaInfo::RegisterMetaFile(MetaFile& meta)
	{
		const auto& it = MetaRegistry.find(meta.Path);
		if (it != MetaRegistry.end())return nullptr;
		const std::string path = meta.Path;
		MetaRegistry.emplace(path, std::move(meta));
		CurFile = &MetaRegistry.find(path)->second;
		return CurFile;
	}
	bool MetaInfo::HasMetaFile(std::string& filePath)
	{
		return MetaRegistry.find(filePath) != MetaRegistry.end();
	}

	void MetaInfo::ClearMetaInfo()
	{
		std::unordered_map<std::string, MetaFile>tempfile;
		MetaRegistry.swap(tempfile);
		CurFile = nullptr;
	}

	std::unordered_map<std::string, reflect::MetaFile>& MetaInfo::GetMetaRegistry()
	{
		return MetaRegistry;
	}
	void MetaInfo::Test()
	{
		for (const auto& metaFile : MetaRegistry)
		{
			std::cout << "mateFile:" << metaFile.second.Path << std::endl;
			for (const auto& metaClass : metaFile.second.Classes)
			{
				std::cout << "\tmateClass:" << metaClass.Name << std::endl;
				for (const auto& metaFunction : metaClass.Functions)
				{
					std::cout << "\tmetaFunction:" << metaFunction.Name << std::endl;
					for (const auto& param : metaFunction.Type_ParamName)
					{
						std::cout << "\t\t-Param:" << param.first << ":" << param.second << std::endl;
					}
				}
				for (const auto& metaProperty : metaClass.Fields)
				{
					std::cout << "\tmetaProperty:" << metaProperty.TypeName << " " << metaProperty.Name << std::endl;
				}
			}
		}
	}

}