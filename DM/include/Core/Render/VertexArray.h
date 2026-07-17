#pragma once
#include"Buffer.h"
#include<Core/MMM/Reference.h>
namespace DM
{
	class VertexBuffer;
	class IndexBuffer;
	class DM_API VertexArray
	{
	public:
		virtual ~VertexArray() = default;
		virtual void Bind() = 0;
		virtual void UnBind() = 0;
		virtual void AddVertexBuffer(const SPtr<VertexBuffer>& vertexBuffer) = 0;
		virtual void SetIndexBuffer(const SPtr<IndexBuffer>& indexBuffer) = 0;
		virtual const std::vector<SPtr<VertexBuffer>>& GetVertexBuffer()const = 0;
		virtual const SPtr<IndexBuffer>& GetIndexBuffer()const = 0;
		static SPtr<VertexArray>Create();
	};
}
