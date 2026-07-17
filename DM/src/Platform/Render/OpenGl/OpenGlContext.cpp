#include "DMPCH.h"
#include "Platform/Render/OpenGl/OpenGlContext.h"
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include"Core/Core.h"
namespace DM
{
	OpenGlContext::OpenGlContext(GLFWwindow* windowHandle):m_WindowHandle(windowHandle)
	{
		DM_CORE_ASSERT(m_WindowHandle, "{}", "windowHandle is null");
	}
	void OpenGlContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int GladStatus = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		DM_CORE_ASSERT(GladStatus, "{}", "Failed to initialze Glad");
	}
	void OpenGlContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}