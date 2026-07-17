#pragma once
#include<DMPCH.h>
#include<glad/glad.h>
#include<Core/Render/Buffer.h>
namespace DM
{
	class DM_API OpenGlVertexBuffer :public VertexBuffer
	{
	public:
		OpenGlVertexBuffer(uint32_t sizeByte);
		OpenGlVertexBuffer(float* vertices, uint32_t elementCount);
		virtual ~OpenGlVertexBuffer() = default;
		virtual void Bind()override;
		virtual void UnBind()override;
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
		virtual void SetData(const void* data, uint32_t size)override;
		virtual BufferLayout GetLayout()const override { return m_Layout; }
	private:
		uint32_t m_Id;
		BufferLayout m_Layout;
	};
	class DM_API OpenGlIndexBuffer :public IndexBuffer
	{
	public:
		OpenGlIndexBuffer(uint32_t* indices, uint32_t elementCount);
		virtual ~OpenGlIndexBuffer() = default;
		virtual void Bind()override;
		virtual void UnBind()override;
		virtual uint32_t GetCount()const override;
	private:
		uint32_t m_Id;
		uint32_t m_Count;

	};
}
