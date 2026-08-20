#include "DMPCH.h"
#ifdef  DM_PLATFORM_WINDOWS
#include<GLFW/glfw3.h>
#include<Platform/Input/Input.h>
namespace DM
{
	Input* Input::m_Inst = nullptr;

	Input::Input() = default;
	Input::~Input() = default;

	bool Input::IsKeyPressed(KeyCode code)
	{
		if (!m_NativeWindow) return false;
		auto state = glfwGetKey(static_cast<GLFWwindow*>(m_NativeWindow), (int)code);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsKeyReleased(KeyCode code)
	{
		if (!m_NativeWindow) return false;
		auto state = glfwGetKey(static_cast<GLFWwindow*>(m_NativeWindow), (int)code);
		return state == GLFW_RELEASE;
	}

	bool Input::IsMouseButtonPressed(MouseButton code)
	{
		if (!m_NativeWindow) return false;
		auto state = glfwGetMouseButton(static_cast<GLFWwindow*>(m_NativeWindow), (int)code);
		return state == GLFW_PRESS;
	}

	Vector2 Input::GetMousePosition()
	{
		if (!m_NativeWindow) return Vector2(-1.0f, -1.0f);
		double x = -1, y = -1;
		glfwGetCursorPos(static_cast<GLFWwindow*>(m_NativeWindow), &x, &y);
		return Vector2((float)x, (float)y);
	}
}
#endif //  DM_PLATFORM_WINDOWS



