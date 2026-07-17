#include<APP.h>
#include<Engine.h>
#include<EngineConfig.h>
#include<EditorLayer.h>
namespace DM
{
	APP::APP()
	{
	}
	APP::~APP()
	{
	}
	void APP::StartUp()
	{
		// 构造启动配置（后续可由命令行参数或 Config.json 覆盖）
		EngineConfig config;
		config.WindowTitle = "DM Engine";
		config.WindowWidth = 1280;
		config.WindowHeight = 720;
		config.VSync = false;

		// DI 工厂：创建并初始化全部子系统
		auto engine = Engine::Create(config);

#ifdef DM_EDITOR
		engine->PushLayer(new EditorLayer("Editor"));
#endif // DM_EDITOR

		engine->RunEditor();
		// engine 超出作用域后自动调用 Shutdown → 析构
	}
}
