#pragma once
#include <string>
#include <cstdint>
#include <Core/Core.h>

namespace DM
{
	struct DM_API EngineConfig
	{
		std::string ConfigFilePath = "Config.json";

		enum class EMode : uint8_t
		{
			Editor,
			Game,
		};
		EMode Mode = EMode::Editor;

		std::string WindowTitle = "DM Engine";
		unsigned int WindowWidth = 1280;
		unsigned int WindowHeight = 720;
		bool VSync = false;
	};
}
