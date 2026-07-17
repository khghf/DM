#include "DMPCH.h"
#include "Platform/Render/OpenGl/Renderer/OpenGlRendererAPI.h"
#include<glad/glad.h>
namespace DM
{
	void OpenGlRendererAPI::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		LOG_CORE_INFO("OpenGlRendererAPI::Init()");
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	}
	void OpenGlRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	}
	void OpenGlRendererAPI::DrawIndexed(const SPtr<VertexArray>& vertexArray, uint32_t indexElementCount)
	{
		uint32_t count = indexElementCount ? indexElementCount : vertexArray->GetIndexBuffer()->GetCount();
		vertexArray->Bind();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}
	void OpenGlRendererAPI::SetClearColor(const Vector4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}
	void OpenGlRendererAPI::SetViewport(uint32_t width, uint32_t height, uint32_t xOffset, uint32_t yOffset)
	{
		glViewport(xOffset, yOffset, width, height);
	}
}
