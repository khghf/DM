#pragma once
#include <string>
#include <Core/Reflection/Mirror/include/mirror.h>
#include <Core/Config/ConfigMgr.h>

namespace DM
{
	
	struct EngineSettings
	{
		std::string WindowTitle = "DM Engine";
		int WindowWidth  = 1280;
		int WindowHeight = 720;
		int WindowPosX   = -1;   // 未记录过位置时为 -1，创建窗口时不做定位
		int WindowPosY   = -1;
		bool Maximized   = false;
		bool VSync       = false;
	};

	REGISTER_TYPE(EngineSettings);
	REGISTER_MEMBER(EngineSettings, WindowTitle);
	REGISTER_MEMBER(EngineSettings, WindowWidth);
	REGISTER_MEMBER(EngineSettings, WindowHeight);
	REGISTER_MEMBER(EngineSettings, WindowPosX);
	REGISTER_MEMBER(EngineSettings, WindowPosY);
	REGISTER_MEMBER(EngineSettings, Maximized);
	REGISTER_MEMBER(EngineSettings, VSync);

	inline static ConfigRegistrar<EngineSettings> g_EngineSettingsRegistrar{ "Engine.json" };
}
