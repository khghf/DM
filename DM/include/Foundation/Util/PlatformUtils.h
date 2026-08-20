#pragma once
#include<iostream>
#include<string.h>
namespace DM
{
	class DM_API FileDialog
	{
	public:
		static std::string OpenFile(const char* filter = "所有文件 (*.*)\0*.*\0");
		static std::string SaveFile(const char* defaultName = "",const char* filter= "所有文件 (*.*)\0*.*\0");
	};
}


