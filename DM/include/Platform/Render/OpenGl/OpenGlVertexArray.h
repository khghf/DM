#pragma once
#include<Core/Render/VertexArray.h>
#include<Core/MMM/Reference.h>
namespace DM
{
	class DM_API OpenGlVertexArray :public VertexArray
	{
	public:
		OpenGlVertexArray();
		~OpenGlVertexArray();
		virtual void Bind() override;
		virtual void UnBind()override;
		virtual void AddVertexBuffer(const SPtr<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const SPtr<IndexBuffer>& indexBuffer) override;
		virtual const std::vector<SPtr<VertexBuffer>>& GetVertexBuffer()const override { return m_VertexBuffers; }
		virtual const SPtr<IndexBuffer>& GetIndexBuffer()const override { return m_IndexBuffer; }
	private:
		std::vector<SPtr<VertexBuffer>>m_VertexBuffers;
		SPtr<IndexBuffer>m_IndexBuffer;
		uint32_t m_Id;
	};
	
}


