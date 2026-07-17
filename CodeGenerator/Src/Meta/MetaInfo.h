#pragma once
#include<string>
#include<vector>
#include<unordered_map>
#include<stdexcept>
//存储通过CodeParser收集的信息，然后用于生成代码
namespace reflect
{
	struct MetaEnum
	{
		std::string Type;
		std::string Name;
		std::vector<std::pair<std::string, int32_t>>Name_Val;
		std::string FilePath;
		bool bIsUnsinged = false;
	};
	struct MetaField
	{
		explicit MetaField(const std::string& name) :Name(name) {}
		std::string Name;
		std::string TypeName;
		std::string ClassName;
		bool IsPublic = false;
	};
	struct MetaFunction
	{
		explicit MetaFunction(const std::string& name) :Name(name) {}
		std::string Name;
		std::string ReturnName;
		std::vector<std::pair<std::string, std::string>>Type_ParamName;
		std::string ClassName;
		bool IsPublic = false;

	};
	struct MetaClass
	{
		explicit MetaClass(const std::string& name) :Name(name) {}
		std::string Name;
		std::vector<MetaFunction>Functions;
		std::vector<MetaField>Fields;
		std::string FilePath;
		std::vector<std::string>NameSpaces;
		uint32_t ReflectBodyLine = 0;
		std::vector <std::pair <std::string, std::string >> SuperClassesNames;//修饰符-类名
	};
	struct MetaFile
	{
		explicit MetaFile(const std::string& name) :Path(name) {}
		std::string Path;
		std::vector<MetaClass>Classes;
		std::vector<MetaEnum>Enums;
		std::vector<uint32_t>ReflectBodyMarcoLine;
		uint32_t CurClassMarcoIndex = 0;
		uint32_t LastIncludeLine = 0;
		bool bHasReflectMark = false;//解析文件中有CLASS()宏则为true,用来避免生成不必要的.gen文件
		MetaClass& GetMetaClass(const std::string&className)
		{
			for (auto& it : Classes)
			{
				if (it.Name == className)return it;
			}
			throw std::runtime_error("No classes to register in"+Path);
		}
		MetaEnum& GetMetaEnum(const std::string& enumName)
		{
			for (auto& it : Enums)
			{
				if (it.Name == enumName)return it;
			}
			throw std::runtime_error("No enum to register in" + Path);
		}
		bool HasMetaClass(const std::string& className)const
		{
			for (const auto& it : Classes)
			{
				if (it.Name == className)return true;
			}
			return false;
		}
		bool HasMetaEnum(const std::string& enumName)const
		{
			for (const auto& it : Enums)
			{
				if (it.Name == enumName)return true;
			}
			return false;
		}

		MetaClass*RegisterMetaClass(MetaClass& meta)
		{
			if (HasMetaClass(meta.Name))
			{
				return &GetMetaClass(meta.Name);
			}
			const std::string name = meta.Name;
			Classes.emplace_back(std::move(meta));
			return &GetMetaClass(name);
		}
		void RegisterMetaFunction(MetaFunction& meta)
		{
			if (!HasMetaClass(meta.ClassName))
			{
				return;
			}
			GetMetaClass(meta.ClassName).Functions.emplace_back(std::move(meta));
		}

		void RegisterMetaProperty(MetaField& meta)
		{
			if (!HasMetaClass(meta.ClassName))
			{
				return;
			}
			GetMetaClass(meta.ClassName).Fields.emplace_back(std::move(meta));
		}
		MetaEnum* RegisterMetaEnum(MetaEnum& meta)
		{
			if (HasMetaEnum(meta.Name))
			{
				return &GetMetaEnum(meta.Name);
			}
			const std::string name = meta.Name;
			Enums.emplace_back(std::move(meta));
			return &GetMetaEnum(name);
		}
	};
	class MetaInfo
	{
	public:
		static MetaFile*		RegisterMetaFile(MetaFile& meta);
		static bool				HasMetaFile(std::string& filePath);
		static void				ClearMetaInfo();
		static std::unordered_map<std::string, MetaFile>& GetMetaRegistry();
		static void			Test();
	};
}
