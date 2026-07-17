#pragma once
#include<Core/RHI/RHI.h>
#include<Core/Math/Vector.h>
#include<Core/MMM/Reference.h>
namespace DM
{
	class VertexArray;
	// RenderCommand - 渲染指令薄层，委托给 RHI::RHIDevice
	class DM_API RenderCommand
	{
	public:
		inline static void Init() { RHI::RHIDevice::Get().Init(); }
		inline static void SetClearColor(const Vector4& color) { RHI::RHIDevice::Get().SetClearColor(color); }
		inline static void Clear() { RHI::RHIDevice::Get().Clear(); }
		inline static void DrawIndexed(const SPtr<VertexArray>& vertexArray,uint32_t indexElementCount=0) { RHI::RHIDevice::Get().DrawIndexed(vertexArray, indexElementCount); }
		inline static void SetViewport(uint32_t width, uint32_t height, uint32_t xOffset = 0, uint32_t yOffset = 0) { RHI::RHIDevice::Get().SetViewport(xOffset, yOffset, width, height); }
	};
}
