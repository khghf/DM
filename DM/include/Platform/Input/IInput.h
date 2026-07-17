#pragma once
#include <Core/Math/Vector.h>
#include <Platform/Input/KeyCode.h>
#include <Platform/Input/MouseButtonCode.h>
#include <Core/Core.h>

namespace DM
{
	// 输入抽象接口（L1 Platform 层）。
	// 取代旧的 Input 全静态方法，由 Engine 创建并注入，
	// 使得上层通过 IInput& 访问输入状态，便于单元测试时替换为 Mock。
	class DM_API IInput
	{
	public:
		virtual ~IInput() = default;

		virtual bool IsKeyPressed(Key code) = 0;
		virtual bool IsKeyReleased(Key code) = 0;
		virtual bool IsMouseButtonPressed(MouseButton code) = 0;
		virtual Vector2 GetMousePosition() = 0;

		// 每帧 BeginFrame/EndFrame：在事件轮询后刷新按下/弹起状态。
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
	};
}
