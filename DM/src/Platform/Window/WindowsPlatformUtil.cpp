#include "DMPCH.h"
#ifdef DM_PLATFORM_WINDOWS
#include<commdlg.h>
#include <Foundation/Util/PlatformUtils.h>
#include <Foundation/Util/Util.h>
#include <string.h>
namespace DM
{
	std::string FileDialog::OpenFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		CHAR currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		if (GetCurrentDirectoryA(256, currentDir))ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE)return ofn.lpstrFile;

		return std::string();
	}
	std::string FileDialog::SaveFile(const char* defaultName,const char* filter )
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		CHAR currentDir[256] = { 0 };
		if (defaultName && strlen(defaultName) > 0)
		{
			strncpy_s(szFile, _countof(szFile), defaultName, _TRUNCATE);
		}
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		if (GetCurrentDirectoryA(256, currentDir))
			ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
		// Sets the default extension by extracting it from the filter
		//ofn.lpstrDefExt = strchr(filter, '\0') + 1;
		if (GetSaveFileNameA(&ofn))return ofn.lpstrFile;
		return std::string();
	}
}
#endif // DM_PLATFORM_WINDOWS

