#pragma once
#include <Core/Layer/LayerStack.h>
#include <Core/EventBus/EventBus.h>
#include <Core/Clock/Clock.h>
#include <Platform/Window/IWindow.h>
#include <Platform/Input/IInput.h>
#include <Core/Core.h>
#include <Core/MMM/Reference.h>

namespace DM
{
	struct EngineConfig;

	// 引擎根对象（L5 之下的中枢）。
	//
	// 设计要点（对应设计报告 Phase 2）：
	//   - 通过 Engine::Create(config) 静态工厂创建，所有子系统在此构造并通过依赖注入。
	//   - 不再暴露全局指针（旧的 extern GEngine* Engine 已移除）。
	//   - 不再使用 friend class APP；启动逻辑完全封装在 Engine 内部，
	//     APP 只调用 RunEditor() / RunGame() / Shutdown()。
	//   - EventBus、Clock、IWindow、IInput、LayerStack 由 Engine 持有并向下注入。
	//   - Config/AssetManager 在迁移期间仍保留静态/单例访问，后续逐步 DI 化。
	class DM_API Engine
	{
	public:
		// —— 静态访问器（过渡期，替代旧的 extern GEngine* Engine）——
		// 仅在 Create() 之后、Shutdown() 之前有效。
		static Engine& Get() { return *s_Instance; }

		// 静态工厂：解析配置、构造并注入所有子系统。
		static UPtr<Engine> Create(const EngineConfig& config);

		virtual ~Engine();

		// 编辑器模式：构造并推入 EditorLayer，然后进入主循环。
		void RunEditor();
		// 游戏模式：推入游戏层后进入主循环。
		void RunGame();

		// 主循环（由 RunEditor/RunGame 内部调用）。
		void Run();
		// 请求关闭（由窗口关闭事件触发）。
		void Close() { m_Running = false; }
		// 关闭：释放子系统。
		void Shutdown();

		// —— 子系统访问（依赖注入的出口）——
		inline IWindow& GetWindow() const { return *m_Window; }
		inline IInput& GetInput() const { return *m_Input; }
		inline EventBus& GetEventBus() const { return *m_EventBus; }
		inline Clock& GetClock() const { return *m_Clock; }
		inline LayerStack& GetLayerStack() { return *m_LayerStack; }

		// 推入/弹出 Layer（编辑器与游戏层都通过此入口）。
		void PushLayer(Layer* layer) { m_LayerStack->PushLayer(layer); }

	protected:
		Engine();

		// 构造全部子系统并注入依赖。由 Create() 调用。
		void Init(const EngineConfig& config);

		// 主循环每帧：时钟、输入、层更新、交换缓冲。
		void OnUpdate(float deltaTime);
		void OnWindowRefresh();

		void PassEvent(class Event* const e);
		void OnWindowResize(class Event* const e);

	private:
		UPtr<IWindow> m_Window;
		UPtr<IInput> m_Input;
		UPtr<EventBus> m_EventBus;
		UPtr<Clock> m_Clock;
		UPtr<LayerStack> m_LayerStack;

		bool m_Running : 1;
		bool m_Minimized : 1;

		static Engine* s_Instance;
	};
}
