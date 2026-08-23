#pragma once
#include"DMPCH.h"
#include"Core/Core.h"
#include"Foundation/MMM/Reference.h"
#include<functional>
namespace DM
{
	struct DM_API WindowProps
	{
		std::string Title;
		unsigned int Width = 1280;
		unsigned int Height = 720;

		bool VSync = false;

		WindowProps() = default;
		WindowProps(const unsigned int width, const unsigned int height, const std::string& title): Title(title), Width(width), Height(height) {}
	};
	class DM_API AppWindow 
	{
	public:
		AppWindow(const WindowProps& Props);
		~AppWindow();
		void Update(float DeltaTime) ;
		static UPtr<AppWindow> Create(const WindowProps& Props = WindowProps());
		bool ShouldClose() ;

		void* GetNativeWindow()const ;

		void SetVSync(bool bEnanle) ;
		bool IsVSyncEnabled()const  { return bIsEnableVSync; }
		int GetWidth()const  { return m_WindowProps.Width; }
		int GetHeight()const  { return m_WindowProps.Height; }
		inline void SetWidth(const int& val) { m_WindowProps.Width = val; }
		inline void SetHeight(const int& val) { m_WindowProps.Height = val; }

		// 设置窗口刷新回调（在窗口 resize/move 期间会被调用，用于强制渲染防止卡顿）
		void SetRefreshCallback(std::function<void()> callback) { m_RefreshCallback = callback; }
	protected:
		void Init(const WindowProps& Props);
		void OnRefresh();
		WindowProps m_WindowProps;
		bool bIsEnableVSync = false;
		std::function<void()> m_RefreshCallback;
	};
}
