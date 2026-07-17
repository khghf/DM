#include<DMPCH.h>
#include"Core/Render/Renderer/Renderer.h"
namespace DM
{
	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData();
	void Renderer::OnWindowResize(uint32_t width, uint32_t height, uint32_t xOffset , uint32_t yOffset )
	{
		RenderCommand::SetViewport(width, height,xOffset, yOffset);
	}
	void Renderer::Init()
	{
		RenderCommand::Init();
	}
	void Renderer::BeginScene(const SPtr<Camera>& orthoCamera)
	{
		m_SceneData->ProjectionViewMartix = orthoCamera->GetProjectionViewMatrix();
	}
	void Renderer::EndScene()
	{
	}
	void Renderer::Submit(const SPtr<Shader>& shader,const SPtr<VertexArray>& vertexArray)
	{
		shader->Bind();
		vertexArray->Bind();
		shader->SetMat4x4("u_ProjectionViewMatrix", m_SceneData->ProjectionViewMartix);
		shader->SetMat4x4("u_ModelMatrix", m_SceneData->ModelMartix);
		shader->SetInt("u_Texture", 0);
		RenderCommand::DrawIndexed(vertexArray);
	}
	RendererAPI::EAPI Renderer::GetAPI() { return RendererAPI::GetAPI(); }
}