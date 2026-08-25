#pragma once
#ifdef DM_PLATFORM_WINDOWS
#if DM_DYNAMIC_LINK
#ifdef EDITOR_BUILD_DLL
#define EDITOR_API __declspec(dllexport)
#else
#define EDITOR_API __declspec(dllimport)
#endif // EDITOR_BUILD_DLL
#else 
#define EDITOR_API
#endif // DM_DYNAMIC_LINK
#else
#error Editor only supports windows
#endif // DM_PLATFORM_WINDOWS








