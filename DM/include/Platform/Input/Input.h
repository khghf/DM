#pragma once
#include<Foundation/Math/Vector.h>
#include<Platform/Input/KeyCode.h>
#include<Platform/Input/MouseButtonCode.h>
#include<Platform/Input/IInput.h>
namespace DM
{
	// GLFW 实现：实现 IInput 抽象接口（L1 Platform 层）。
	// 由 Engine 创建并注入；不再使用全静态方法，便于单元测试替换为 Mock。
	class DM_API Input : public IInput
	{
	public:
		Input();
		virtual ~Input();

		// Engine 在创建窗口后注入原生窗口指针。
		void SetNativeWindow(void* window) { m_NativeWindow = window; }

		virtual bool IsKeyPressed(KeyCode code) override;
		virtual bool IsKeyReleased(KeyCode code) override;
		virtual bool IsMouseButtonPressed(MouseButton code) override;
		virtual Vector2 GetMousePosition() override;

		virtual void BeginFrame() override {}
		virtual void EndFrame() override {}

		// 临时静态桥接（旧代码兼容，待所有调用方迁移后移除）
		static Input* m_Inst;
	private:
		void* m_NativeWindow = nullptr;
	};
}
