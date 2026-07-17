#pragma once
#include<Core/Render/Renderer/RendererAPI.h>
#include<Core/Math/Vector.h>
namespace DM
{
	class DM_API OpenGlRendererAPI :public RendererAPI
	{
	public:
		virtual void Init()override;
		virtual void Clear() override;
		virtual void DrawIndexed(const SPtr<VertexArray>& vertexArray, uint32_t indexElementCount = 0) override;
		virtual void SetClearColor(const Vector4& color) override;
		virtual void SetViewport(uint32_t width, uint32_t height, uint32_t xOffset = 0, uint32_t yOffset = 0)override;
	};
}
