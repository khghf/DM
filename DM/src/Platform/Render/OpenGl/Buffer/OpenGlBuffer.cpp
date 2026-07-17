#include<DMPCH.h>
#include "Platform/Render/OpenGl/Buffer/OpenGlBuffer.h"
#include<glad/glad.h>
namespace DM
{
	OpenGlVertexBuffer::OpenGlVertexBuffer(uint32_t sizeByte)
	{
		glGenBuffers(1, &m_Id);
		glBindBuffer(GL_ARRAY_BUFFER, m_Id);
		glBufferData(GL_ARRAY_BUFFER, sizeByte, nullptr, GL_DYNAMIC_DRAW);
	}
	/////////////////////////////////////////////////////////////VertexBuffer///////////////////////////////////////////////////////////////
	OpenGlVertexBuffer::OpenGlVertexBuffer(float* vertices, uint32_t elementCount)
	{
		glGenBuffers(1, &m_Id);
		glBindBuffer(GL_ARRAY_BUFFER, m_Id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * elementCount, vertices, GL_STATIC_DRAW);
	}
	void OpenGlVertexBuffer::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_Id);
	}
	void OpenGlVertexBuffer::UnBind()
	{
		glBindBuffer(GL_ARRAY_BUFFER,0);
	}
	void OpenGlVertexBuffer::SetData(const void* data, uint32_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_Id);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}
	/////////////////////////////////////////////////////////////IndexBuffer///////////////////////////////////////////////////////////////
	OpenGlIndexBuffer::OpenGlIndexBuffer(uint32_t* indices, uint32_t elementCount):m_Count(elementCount)
	{
		glGenBuffers(1, &m_Id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * elementCount, indices, GL_STATIC_DRAW);
	}
	void OpenGlIndexBuffer::Bind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Id);
	}
	void OpenGlIndexBuffer::UnBind()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	uint32_t OpenGlIndexBuffer::GetCount() const
	{
		return m_Count;
	}
}