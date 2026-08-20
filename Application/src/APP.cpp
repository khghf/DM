#include<APP.h>
#include<Engine.h>
#include<EngineConfig.h>

#ifdef DM_EDITOR
#include<Editor.h>
#endif // DM_EDITOR
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
		EngineConfig config;
		config.WindowTitle = "DM Engine";
		config.WindowWidth = 1280;
		config.WindowHeight = 720;
		config.VSync = false;

		Engine*engine=Engine::Create();

		engine->Init(config);
#ifdef DM_EDITOR
		Engine::Get()->PushLayer(Editor::Get());
#endif // DM_EDITOR
		engine->Run();
	}
}
