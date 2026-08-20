#pragma once
#include <Core/Layer/LayerStack.h>
#include <Core/Clock/Clock.h>
#include <Platform/Window/AppWindow.h>
#include <Platform/Input/IInput.h>
#include <Core/Core.h>
#include <Foundation/MMM/Reference.h>
#include<Core/Renderer/Renderer.h>
namespace DM
{
	struct EngineConfig;
	struct Event;
	class DM_API Engine
	{
		friend class APP;
		friend class EventBus;
	private:
		Engine();
	public:
		virtual ~Engine();

		static Engine* Create();
		void Close() { m_Running = false; }

		static Engine* Get() { return m_Inst; }
		static Renderer* GetRenderer() { return m_Inst->m_Renderer.get(); }

	protected:
		void Init(const EngineConfig& config);

		void Run();
		/*void RunEditor();
		void RunGame();*/
		void OnFrameBegin();
		void OnUpdate(float deltaTime);
		void OnFrameEnd();

		void Shutdown();


		void HandleEvent(Event* const e);
		void PassEvent( Event* const e);
		void OnWindowResize( Event* const e);

	public:
		inline AppWindow& GetAppWindow() const { return *m_Window; }
		inline IInput& GetInput() const { return *m_Input; }
		inline Clock& GetClock() const { return *m_Clock; }
		inline LayerStack& GetLayerStack() { return *m_LayerStack; }

		void PushLayer(Layer* layer) { m_LayerStack->PushLayer(layer); }

	private:
		UPtr<AppWindow> m_Window;
		UPtr<IInput> m_Input;
		UPtr<Clock> m_Clock;
		UPtr<LayerStack> m_LayerStack;

		UPtr <Renderer> m_Renderer;

		bool m_Running : 1;
		bool m_Minimized : 1;

		static Engine* m_Inst;
	};
}
