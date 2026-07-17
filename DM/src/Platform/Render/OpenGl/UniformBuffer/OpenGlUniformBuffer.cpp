#include "DMPCH.h"
#include "Platform/Render/OpenGl/UniformBuffer/OpenGlUniformBuffer.h"

#include <glad/glad.h>
namespace DM
{
	OpenGlUniformBuffer::OpenGlUniformBuffer(uint32_t size, uint32_t binding)
	{
		glCreateBuffers(1, &m_Id);
		glNamedBufferData(m_Id, size, nullptr, GL_DYNAMIC_DRAW); 
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_Id);
	}
	OpenGlUniformBuffer::~OpenGlUniformBuffer()
	{
		glDeleteBuffers(1, &m_Id);
	}
	void OpenGlUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		glNamedBufferSubData(m_Id, offset, size, data);
	}
}