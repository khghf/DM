#pragma once
#include<Foundation/Math/Vector.h>
#include<Platform/Input/KeyCode.h>
#include<Platform/Input/MouseButtonCode.h>
#include<Platform/Input/IInput.h>
namespace DM
{
	class DM_API Input : public IInput
	{
	public:
		Input();
		virtual ~Input();

		void SetNativeWindow(void* window) { m_NativeWindow = window; }

		virtual bool IsKeyPressed(KeyCode code) override;
		virtual bool IsKeyReleased(KeyCode code) override;
		virtual bool IsMouseButtonPressed(MouseButton code) override;
		virtual Vector2 GetMousePosition() override;

		virtual void BeginFrame() override {}
		virtual void EndFrame() override {}

		static Input* m_Inst;
	private:
		void* m_NativeWindow = nullptr;
	};
}
