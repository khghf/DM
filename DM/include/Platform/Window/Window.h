#pragma once
#include"DMPCH.h"
#include"Core/Core.h"
#include"Core/MMM/Reference.h"
#include"Platform/Window/IWindow.h"
#include<functional>
namespace DM
{
	class DM_API Window : public IWindow
	{
	public:
		Window(const WindowProps& Props);
		virtual ~Window();
		virtual void Update(float DeltaTime) override;
		static UPtr<Window> Create(const WindowProps& Props = WindowProps());
		virtual bool ShouldClose() override;

		virtual void* GetNativeWindow()const override;

		virtual void SetVSync(bool bEnanle) override;
		virtual bool IsVSyncEnabled()const override { return bIsEnableVSync; }
		virtual int GetWidth()const override { return m_WindowProps.Width; }
		virtual int GetHeight()const override { return m_WindowProps.Height; }
		inline void SetWidth(const int& val) { m_WindowProps.Width = val; }
		inline void SetHeight(const int& val) { m_WindowProps.Height = val; }

		// 设置窗口刷新回调（在窗口 resize/move 期间会被调用，用于强制渲染防止卡顿）
		void SetRefreshCallback(std::function<void()> callback) { m_RefreshCallback = std::move(callback); }
	protected:
		virtual void Init(const WindowProps& Props);
		void OnRefresh();
		WindowProps m_WindowProps;
		bool bIsEnableVSync = false;
		std::function<void()> m_RefreshCallback;
	};
}
