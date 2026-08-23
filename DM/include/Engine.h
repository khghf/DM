#pragma once
#include <Core/Layer/LayerStack.h>
#include <Core/Clock/Clock.h>
#include <Platform/Window/AppWindow.h>
#include <Platform/Input/IInput.h>
#include <Core/Core.h>
#include <Foundation/MMM/Reference.h>
#include<Core/Renderer/Renderer.h>
#include<Core/GameInst.h>
namespace DM
{
	struct EngineConfig;
	struct Event;
	class GameInst;
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

		void OnFrameBegin();
		void OnUpdate(float deltaTime);
		void OnFrameEnd();

		void Shutdown();

		void HandleEvent(Event* const e);
		void PassEvent( Event* const e);

		void OnWindowResize( Event* const e);
	public:
		AppWindow&	GetAppWindow() const	{ return *m_Window; }
		IInput&		GetInput() const		{ return *m_Input; }
		Clock&		GetClock() const		{ return *m_Clock; }
		LayerStack&	GetLayerStack()			{ return *m_LayerStack; }
		GameInst&	GetGameInst()const		{ return *m_GameInst; }

		void PushLayer(Layer* layer) { m_LayerStack->PushLayer(layer); }

	private:
		UPtr<AppWindow>		m_Window;
		UPtr<IInput>		m_Input;
		UPtr<Clock>			m_Clock;
		UPtr<LayerStack>	m_LayerStack;
		UPtr<Renderer>		m_Renderer;
		UPtr<GameInst>		m_GameInst;

		bool m_Running;
		bool m_Minimized;
		static Engine* m_Inst;
	};
}
