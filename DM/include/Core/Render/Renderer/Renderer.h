#pragma once
#include<Framework/Camera/OrthoGraphicCamera.h>
#include"../Shader.h"
#include"RenderCommand.h"
#include"RendererAPI.h"
namespace DM
{
	class OrthoGraphicCamera;
	class DM_API Renderer
	{
	public:
		static void OnWindowResize(uint32_t width, uint32_t height,uint32_t xOffset=0, uint32_t yOffset=0);
		static void Init();
		static void BeginScene(const SPtr<Camera>& camera);
		static void EndScene();
		static void Submit(const SPtr<Shader>& shader,const SPtr<VertexArray>& vertexArray);
		static RendererAPI::EAPI GetAPI();
	private:
		struct SceneData
		{
			Matrix4 ProjectionViewMartix= Matrix4(1.f);
			Matrix4 ModelMartix = Matrix4(1.f);
		};
		static SceneData* m_SceneData;
	};
}
