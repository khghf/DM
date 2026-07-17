#pragma once
#include<Core/Render/GraphicsContext.h>
struct GLFWwindow;
namespace DM
{
	class DM_API OpenGlContext:public GraphicsContext
	{
	public:
		explicit OpenGlContext(GLFWwindow* windowHandle);
		virtual void Init();
		virtual void SwapBuffers();
	private:
		GLFWwindow* m_WindowHandle;
	};
}


