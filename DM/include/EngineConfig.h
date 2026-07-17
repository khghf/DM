#pragma once
#include <string>
#include <cstdint>
#include <Core/Core.h>

namespace DM
{
	// 引擎启动配置（由命令行参数 / 配置文件构造，注入 Engine）。
	// 取代旧的"全静态 Config + 全局指针"方案。
	struct DM_API EngineConfig
	{
		// 配置文件（Config.json）的相对或绝对路径。
		// 默认相对路径从工作目录（项目根目录）开始。
		std::string ConfigFilePath = "Config.json";

		// 启动模式。
		enum class EMode : uint8_t
		{
			Editor,
			Game,
		};
		EMode Mode = EMode::Editor;

		// 窗口初始属性。
		std::string WindowTitle = "DM Engine";
		unsigned int WindowWidth = 1280;
		unsigned int WindowHeight = 720;
		bool VSync = false;
	};
}
