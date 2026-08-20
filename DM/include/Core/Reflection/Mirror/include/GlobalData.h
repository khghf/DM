#pragma once
#include<unordered_map>
#include"ReflectMarco.h"
#include"Info/TypeInfo.h"
#include"Info/FunctionInfo.h"
namespace mirror
{
	/*
	存储全局反射信息
	*/
	struct REFLECT_API GlobalData
	{
		GlobalData() = default;
		~GlobalData() = default;
		GlobalData(const GlobalData&) = delete;
		GlobalData(GlobalData&&) noexcept = delete;
		GlobalData& operator=(const GlobalData&) = delete;
		GlobalData& operator=(GlobalData&&) noexcept = delete;

		std::unordered_map<TypeId, TypeInfo>			TypeInfoMap{ };

		std::unordered_map<TypeId, EnumInfo>			EnumInfoMap{ };

		std::unordered_map<std::string, TypeId>			NameToTypeIdMap{ };

		std::unordered_map<const void*, TypeId>			VTableToTypeIdMap{ };

		std::unordered_map<FunctionId, FunctionInfo>	FunctionInfoMap{ };

		std::unordered_map<std::string, FunctionId>		NameToFunctionIdMap{ };

		std::unordered_map<const void*, FunctionId>		FunctionAddressToIdMap{ };
	};
	REFLECT_API GlobalData& GetGlobalData();
}
