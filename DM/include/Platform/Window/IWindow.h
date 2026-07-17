#pragma once
#include <string>
#include <functional>
#include <Core/Core.h>

namespace DM
{
	// 窗口属性。
	struct DM_API WindowProps
	{
		std::string Title;
		unsigned int Width = 1280;
		unsigned int Height = 720;
		bool VSync = false;
		WindowProps() = default;
		WindowProps(const unsigned int width, const unsigned int height, const std::string& title)
			: Title(title), Width(width), Height(height) {}
	};

	// 窗口抽象接口（L1 Platform 层）。
	// 上层代码只依赖 IWindow，不 include 任何平台特定头文件。
	// 当前唯一实现为 GLFW（见 WindowGLFW），后续可无缝替换为 Win32/SDL。
	class DM_API IWindow
	{
	public:
		virtual ~IWindow() = default;

		virtual void Update(float deltaTime) = 0;
		virtual bool ShouldClose() = 0;
		virtual void* GetNativeWindow() const = 0;

		virtual void SetVSync(bool enable) = 0;
		virtual bool IsVSyncEnabled() const = 0;

		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;

		// 窗口刷新回调：resize/move 期间由平台触发，用于强制渲染防止卡顿。
		virtual void SetRefreshCallback(std::function<void()> callback) = 0;
	};
}
