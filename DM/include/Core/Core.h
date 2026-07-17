#pragma once
#ifdef DM_PLATFORM_WINDOWS
	#if DM_DYNAMIC_LINK
		#ifdef DM_BUILD_SHARED
			#define DM_API __declspec(dllexport)
		#else
			#define DM_API __declspec(dllimport)
		#endif // DM_BUILD_SHARED
	#else 
		#define DM_API
	#endif // DM_DYNAMIC_LINK
#else
	#error DM only supports windows
#endif // DM_PLATFORM_WINDOWS