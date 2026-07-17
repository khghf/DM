#include<DMPCH.h>
#include "Platform/Render/OpenGl/OpenGlVertexArray.h"
#include<Tool/Util/GameStatic.h>
#include<Core/Log.h>
#include<glad/glad.h>
#include<Core/MMM/Reference.h>

namespace DM
{
	static GLenum ShaderDataTypeToOpenGlType(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::None:		DM_CORE_ASSERT(false, "{}", "Unknown ShaderDataType!"); return 0;
			case ShaderDataType::Float:		return GL_FLOAT;
			case ShaderDataType::Float2:	return GL_FLOAT;
			case ShaderDataType::Float3:	return GL_FLOAT;
			case ShaderDataType::Float4:	return GL_FLOAT;
			case ShaderDataType::Int:		return GL_INT;
			case ShaderDataType::Int2:		return GL_INT;
			case ShaderDataType::Int3:		return GL_INT;
			case ShaderDataType::Int4:		return GL_INT;
			case ShaderDataType::Mat3x3:	return GL_FLOAT;
			case ShaderDataType::Mat4x4:	return GL_FLOAT;
			case ShaderDataType::Bool:		return GL_BOOL;
		}
		DM_CORE_ASSERT(false, "{}", "Unknown ShaderDataType!");
		return 0;
	}
	OpenGlVertexArray::OpenGlVertexArray()
	{
		glGenVertexArrays(1, &m_Id);
		glBindVertexArray(m_Id);
	}
	OpenGlVertexArray::~OpenGlVertexArray()
	{
		glDeleteVertexArrays(1, &m_Id);
	}
	void OpenGlVertexArray::Bind()
	{
		glBindVertexArray(m_Id);
	}
	void OpenGlVertexArray::UnBind()
	{
		glBindVertexArray(0);
	}
	void OpenGlVertexArray::AddVertexBuffer(const SPtr<VertexBuffer>& vertexBuffer)
	{
		DM_CORE_ASSERT(vertexBuffer->GetLayout().GetBufferElements().size(), "{}", "Vertex Buffer has no layout!");
		glBindVertexArray(m_Id);
		vertexBuffer->Bind();
		const auto& layout = vertexBuffer->GetLayout();
		int location = 0;
		for (const auto& Element : layout)
		{
			if (ShaderDataTypeToOpenGlType(Element.Type) == GL_INT)
			{
				glVertexAttribIPointer(
					location,
					Element.GetComponentCount(),
					ShaderDataTypeToOpenGlType(Element.Type),
					layout.GetStride(),
					reinterpret_cast<void*>(Element.Offset)
				);
			}
			else
			{
				glVertexAttribPointer(
					location,
					Element.GetComponentCount(),
					ShaderDataTypeToOpenGlType(Element.Type),
					Element.bNormailzed ? GL_TRUE : GL_FALSE,
					layout.GetStride(),
					reinterpret_cast<void*>(Element.Offset)
				);
			}
			glEnableVertexAttribArray(location);
			++location;
		}
		m_VertexBuffers.push_back(vertexBuffer);
	}
	void OpenGlVertexArray::SetIndexBuffer(const SPtr<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_Id);
		indexBuffer->Bind();
		m_IndexBuffer = indexBuffer;
	}
}

