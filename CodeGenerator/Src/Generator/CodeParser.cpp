#include "CodeParser.h"
#include<iostream>
#include"../Util.h"
#include"../Meta/MetaInfo.h"
#include<sstream>
#include<regex>
#include <windows.h>
#include <vector>
#include<string>
using namespace std;

namespace reflect
{
	struct ParseContext
	{
		CXCursor CurrrentClassNode;
		MetaFile* CurMetaFile = nullptr;
		MetaClass* CurMetaClass = nullptr;
	};
	constexpr bool bPrintInfo = true;
	ParseContext Context;
	static CXChildVisitResult CursorVisitor(CXCursor cursor, CXCursor parent, CXClientData client_data);
	static CXChildVisitResult ParseClass(CXCursor classNode);
	static CXChildVisitResult ParseFunction(CXCursor functionNode);
	static CXChildVisitResult ParseField(CXCursor propertyNode);
	//static CXChildVisitResult ParseStruct(CXCursor structNode);结构体直接套用类的解析函数
	static CXChildVisitResult ParseEnum(CXCursor enumNode);
	CodeParser::CodeParser() :m_Index(nullptr)
	{
		m_Index = clang_createIndex(0, 0);
		if (m_Index == nullptr)
		{
			std::cerr << "Create CXIndex failed:" << __FILE__<< std::endl;
			
			return;
		}
	}

	CodeParser::~CodeParser()
	{
		if (m_Index)clang_disposeIndex(m_Index);
	}
	static std::vector<const char*>args =
	{
		/*"--include=map",
		"--include=unordered_map",
		"--include=set",
		"--include=unordered_set",
		"--include=string",
		"--include=string_view",
		"--include=vector",
		"--include=array",
		"--include=queue",
		"--include=dequeue",
		"--include=memory",
		"--include=utility",*/
		//"--include=D:/Projects/DM/DM/Src/Core/MMM/Reference.h",
		//"-ID:/Projects/DM/DM/Src/Core/MMM",
	};
	void CodeParser::Parse()
	{
		std::string arg;
		if (!m_ProjectRoot.empty())
		{
			arg= "-I" + m_ProjectRoot + "/" + m_IncludeDir;
			args.emplace_back(arg.c_str());
		}
		args.emplace_back("-x");
		args.emplace_back("c++-header");
		args.emplace_back("-std=c++20");
		args.emplace_back("-D__clang__");
		args.emplace_back("-DENABLE_PARSER");
		args.emplace_back("-MG");
		args.emplace_back("-M");
		args.emplace_back("-ferror-limit=0");
		args.push_back("-o clangLog.txt");
		args.emplace_back("--include=Src/ReflectMarco.h");
		std::vector<std::string>filePaths=Util::GetAllFilePath(m_ScanPaths, true, m_ExcludePaths);
		/*for (const auto& path : m_ScanPaths)
		{
			std::vector<std::string> p = Util::GetAllFilePath(m_ScanPaths, true,m_ExcludePaths);
			filePaths.insert(filePaths.end(), p.begin(), p.end());
		}*/
		for (const auto& path : filePaths)
		{
			if (Util::GetFileSuffix(path) != "h")continue;
			std::cout << path << std::endl;
			Parse(path);
		}
	}
	
	void CodeParser::Parse(const std::string_view& filePath)
	{
		CXTranslationUnit unit; 
		unsigned options = CXTranslationUnit_DetailedPreprocessingRecord | CXTranslationUnit_SkipFunctionBodies;
		CXErrorCode errorCode = clang_parseTranslationUnit2(m_Index, filePath.data(), args.data(), args.size(), nullptr, 0, options, &unit);
		if (errorCode != CXError_Success)
		{
			switch (errorCode)
			{
			case CXError_Failure:std::cerr << "Create CXTranslationUnit:CXError_Failure" << __FILE__ << std::endl;
				break;
			case CXError_Crashed:std::cerr << "Create CXTranslationUnit:CXError_Crashed" << __FILE__ << std::endl;
				break;
			case CXError_InvalidArguments:std::cerr << "Create CXTranslationUnit:CXError_InvalidArguments" << __FILE__ << std::endl;
				break;
			case CXError_ASTReadError:std::cerr << "Create CXTranslationUnit:CXError_ASTReadError" << __FILE__ << std::endl;
				break;
			default:
				break;
			}
			return;
		}

		MetaFile meta(std::string(filePath.data()));
		meta.LastIncludeLine = Util::GetLastIncludeInFile(Util::ModifySuffix(meta.Path,"h"));
		Context.CurMetaFile = &meta;
		Context.CurMetaClass = nullptr;
		CXCursor root = clang_getTranslationUnitCursor(unit);
		clang_visitChildren(root, CursorVisitor, nullptr);
		if (meta.bHasReflectMark)
		{
			MetaInfo::RegisterMetaFile(meta);
		}
		clang_disposeTranslationUnit(unit);
	}

	std::vector<std::string> CodeParser::GetClassNameSpace(CXCursor classNode)
	{
		std::vector<std::string> res;
		CXCursor CurCursor = classNode;
		while (true) {
			// 获取当前节点的语义父节点
			CXCursor parentCursor = clang_getCursorSemanticParent(CurCursor);
			if (clang_getCursorKind(parentCursor) == CXCursor_TranslationUnit) {
				break;
			}
			// 判断父节点是否为命名空间
			if (clang_getCursorKind(parentCursor) == CXCursor_Namespace) {
				// 获取命名空间名称并存入向量
				res.push_back(Util::CursorSpelling(parentCursor));
			}
			// 向上移动到父节点，继续回溯
			CurCursor = parentCursor;
		}
		return res;
	}

	CXChildVisitResult CursorVisitor(CXCursor cursor, CXCursor parent, CXClientData client_data)
	{
		//跳过对主文件和ReflectMarco.h以外的文件的解析,防止生成其它头文件的反射信息
		if (!Util::IsTargetHeaderCursor(cursor, "ReflectMarco.h") && !Util::IsTargetHeaderCursor(cursor, Util::GetFileName(Context.CurMetaFile->Path, true)))
		{
			return CXChildVisit_Continue;
		}
		CXCursorKind c_kind = clang_getCursorKind(cursor);
		//std::cout << Util::DisplayName(cursor)<< std::endl;
		if (c_kind == CXCursor_AnnotateAttr)
		{
			std::string attrStr = Util::DisplayName(cursor);
			
			if (attrStr=="reflect_class")
			{
				Context.CurrrentClassNode = parent;
				Context.CurMetaFile->bHasReflectMark = true;
				return ParseClass(parent);
			}
			else if (attrStr=="reflect_function")
			{
				return ParseFunction(parent);
			}
			else if (attrStr=="reflect_property")
			{
				return ParseField(parent);
			}
			else if (attrStr == "reflect_struct")
			{
				Context.CurrrentClassNode = parent;
				Context.CurMetaFile->bHasReflectMark = true;
				return ParseClass(parent);
			}
			else if (attrStr == "reflect_enum")
			{
				Context.CurMetaFile->bHasReflectMark = true;
				ParseEnum(parent);
			}
			else
			{
				return CXChildVisit_Recurse;
			}
		}
		//记录REFLECT_BODY()宏的位置
		if (c_kind == CXCursor_MacroExpansion)
		{
			if (Util::DisplayName(cursor) == "REFLECT_BODY")
			{
				uint32_t line = 0, col = 0;
				Util::GetCursorLocation(cursor, &line, &col);

				if (Context.CurMetaFile)
				{
					Context.CurMetaFile->ReflectBodyMarcoLine.push_back(line);
				}
			}
		}
		//获取父类名字
		if (c_kind == CXCursor_CXXBaseSpecifier)
		{
			if (Context.CurMetaClass)
			{
				CX_CXXAccessSpecifier AccessSpecifier=	clang_getCXXAccessSpecifier(cursor);
				std::string accessName;
				switch (AccessSpecifier)
				{
				case CX_CXXInvalidAccessSpecifier:accessName = "None";break;
				case CX_CXXPublic:accessName = "public";break;
				case CX_CXXProtected:accessName = "protected";break;
				case CX_CXXPrivate:accessName = "private";break;
				}
				std::string superClassName = Util::GetStrBehindLastChar(Util::DisplayName(cursor), ":");
				Context.CurMetaClass->SuperClassesNames.emplace_back(accessName, superClassName);
			}
		}
		return CXChildVisit_Recurse;
	}




	CXChildVisitResult ParseClass(CXCursor classNode)
	{
		if (!clang_isCursorDefinition(classNode))return CXChildVisit_Recurse;
		std::string className = Util::DisplayName(classNode);
		MetaClass meta(className);
		meta.FilePath = Context.CurMetaFile->Path;
		meta.NameSpaces=CodeParser::GetClassNameSpace(classNode);
		if (Context.CurMetaFile->ReflectBodyMarcoLine.size() > 0)
		{
			//记录类中声明的REFLECT_BODY()宏的位置
			const int index = Context.CurMetaFile->CurClassMarcoIndex++;
			if(index< Context.CurMetaFile->ReflectBodyMarcoLine.size())
			meta.ReflectBodyLine = Context.CurMetaFile->ReflectBodyMarcoLine[index];
		}
		Context.CurMetaClass= Context.CurMetaFile->RegisterMetaClass(meta);
		if(bPrintInfo)std::cout << "class " + className << std::endl;
		return CXChildVisit_Recurse;
	}


	CXChildVisitResult ParseFunction(CXCursor functionNode)
	{
		std::string funName = Util::CursorSpelling(functionNode);
		MetaFunction meta(funName);
		meta.ClassName = Util::DisplayName(Context.CurrrentClassNode);

		CX_CXXAccessSpecifier AccessSpecifier = clang_getCXXAccessSpecifier(functionNode);
		if (AccessSpecifier == CX_CXXPublic)meta.IsPublic = true;

		auto funVisitor = [](CXCursor paramCursor, CXCursor parent, CXClientData client_data) -> CXChildVisitResult {
			CXCursorKind paramKind = clang_getCursorKind(paramCursor);
			if (paramKind == CXCursor_ParmDecl)
			{
				std::string paramTypeStr = Util::CursorTypeSpelling(paramCursor);
				std::string paramNameStr = Util::DisplayName(paramCursor);
				MetaFunction* m = static_cast<MetaFunction*>(client_data);
				m->Type_ParamName.emplace_back(std::make_pair(paramTypeStr, paramNameStr));
			}
			return CXChildVisit_Continue;
			};
		clang_visitChildren(functionNode, funVisitor, &meta);
		Context.CurMetaFile->RegisterMetaFunction(meta);
		if (bPrintInfo)
		{
			std::cout << funName << std::endl;
		}
		return CXChildVisit_Recurse;
	}


	CXChildVisitResult ParseField(CXCursor propertyNode)
	{
		auto type=clang_getCursorType(propertyNode);
		std::string name = Util::CursorSpelling(propertyNode);
		std::string typeNmae = Util::CursorTypeSpelling(propertyNode);
		MetaField meta(name);
		meta.TypeName = typeNmae;
		meta.ClassName = Util::DisplayName(Context.CurrrentClassNode);


		CX_CXXAccessSpecifier AccessSpecifier = clang_getCXXAccessSpecifier(propertyNode);
		if (AccessSpecifier == CX_CXXPublic)meta.IsPublic = true;

		Context.CurMetaFile->RegisterMetaProperty(meta);
		if (bPrintInfo)
		{
			std::cout << typeNmae + " " + name << std::endl;

		}
		return CXChildVisit_Recurse;
	}


	CXChildVisitResult ParseEnum(CXCursor enumNode)
	{
		MetaEnum meta;
		meta.Type = Util::TypeSpelling(clang_getEnumDeclIntegerType(enumNode));
		meta.Name = Util::DisplayName(enumNode);
		meta.FilePath = Context.CurMetaFile->Path;
		if (meta.Type[0] == 'u')meta.bIsUnsinged = true;
		auto vis = [](CXCursor cursor, CXCursor parent, CXClientData client_data)->CXChildVisitResult
			{
				CXCursorKind cursorKind = clang_getCursorKind(cursor);
				MetaEnum* meta = (static_cast<MetaEnum*>(client_data));
				if (cursorKind == CXCursor_EnumConstantDecl) {
					std::string name = Util::DisplayName(cursor);
					int32_t val;
					if (meta->bIsUnsinged)
					{
						val = clang_getEnumConstantDeclUnsignedValue(cursor);
					}
					else
					{
						val = clang_getEnumConstantDeclValue(cursor);
					}
					if(bPrintInfo)std::cout <<"\t"<< name << "=" << val << std::endl;
					meta->Name_Val.emplace_back(name, val);
				}
				return CXChildVisit_Recurse;
			};
		if (bPrintInfo)
		{
			std::cout << "enum\n";
		}
		clang_visitChildren(enumNode, vis,&meta);
		Context.CurMetaFile->RegisterMetaEnum(meta);
		return CXChildVisit_Continue;
	}

}

