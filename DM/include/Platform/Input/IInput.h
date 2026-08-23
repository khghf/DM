#pragma once
#include <Foundation/Math/Vector.h>
#include <Platform/Input/KeyCode.h>
#include <Platform/Input/MouseButtonCode.h>
#include <Core/Core.h>

namespace DM
{
	class DM_API IInput
	{
	public:
		virtual ~IInput() = default;

		virtual bool IsKeyPressed(KeyCode code) = 0;
		virtual bool IsKeyReleased(KeyCode code) = 0;
		virtual bool IsMouseButtonPressed(MouseButton code) = 0;
		virtual Vector2 GetMousePosition() = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
	};
}
