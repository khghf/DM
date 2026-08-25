#include<APP.h>
#include<Engine.h>
#include<EngineConfig.h>
#include<Core/Config/ConfigMgr.h>
#include<Core/Config/EngineSettings.h>

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
		// 统一路径基准与外部配置加载(Config/Engine.json、Config/Style.json 等)
		Paths::Init();
		ConfigMgr::Init();

		EngineConfig config;
		const auto& es = ConfigMgr::Get<EngineSettings>();
		config.WindowTitle  = es.WindowTitle;
		config.WindowWidth  = (unsigned int)es.WindowWidth;
		config.WindowHeight = (unsigned int)es.WindowHeight;
		config.VSync        = es.VSync;

		Engine*engine=Engine::Create();

		engine->Init(config);
#ifdef DM_EDITOR
		Engine::Get()->PushLayer(Editor::Get());
#endif // DM_EDITOR
		engine->Run();
	}
}
