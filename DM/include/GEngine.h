#pragma once
#include<Platform/Window/Window.h>
#include"Core/Layer/LayerStack.h"
#include<Core/EventSystem/Event/Event.h>
#include<Core/MMM/Reference.h>
namespace DM {
	class APP;
	class DM_API GEngine
	{
	public:
		friend class APP;
		GEngine();
		virtual ~GEngine();
		void Run();
		inline Window const* GetWindow()const { return m_Win.get(); }
		void PushLayer(Layer* layer) { m_LayerStack->PushLayer(layer); }
		//void PushOverLayer(Layer* layer) { m_LayerStack->PushOverLayer(layer); }
		void Close() { bRunning = false; }
		void Init();
	private:
		virtual void OnStart();
		virtual void Update(float DeltaTime);
		virtual void OnExit();



		void PassEvent(Event* const e);
		void OnWindowResize(Event* const e);
		void OnWindowRefresh();
	protected:
		UPtr<LayerStack>m_LayerStack;
	private:
		SPtr<Window>m_Win;
		bool bRunning :1;
		bool bMinimized:1;
		static bool bAlreadyInit;
	};
	DM_API extern GEngine* Engine;
}
