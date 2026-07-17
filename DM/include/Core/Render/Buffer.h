#pragma once
#include<Core/Log.h>
#include<Core/MMM/Reference.h>
namespace DM
{
	enum class ShaderDataType :uint8_t
	{
		None,
		Float,	Float2,	Float3,	Float4,
		Int,	Int2,	Int3,	Int4,
		Mat3x3,	Mat4x4,
		Bool,
	};
	static uint32_t GetShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::None:		return 0;
		case ShaderDataType::Float:		return 4;
		case ShaderDataType::Float2:	return 8;
		case ShaderDataType::Float3:	return 12;
		case ShaderDataType::Float4:	return 16;
		case ShaderDataType::Int:		return 4;
		case ShaderDataType::Int2:		return 8;
		case ShaderDataType::Int3:		return 12;
		case ShaderDataType::Int4:		return 16;
		case ShaderDataType::Mat3x3:	return 36;
		case ShaderDataType::Mat4x4:	return 64;
		case ShaderDataType::Bool:		return 1;
		}
		DM_CORE_ASSERT(false, "{}", "Unknown ShaderDataType!");
		return 0;
	}
	struct DM_API BufferElement
	{
		ShaderDataType Type;
		std::string_view Name;
		uint32_t Size_byte;
		uint32_t Offset;
		bool bNormailzed;
		BufferElement(ShaderDataType type, std::string_view name,bool normalized=false):
		Type(type),Name(name), Size_byte(GetShaderDataTypeSize(type)),Offset(0), bNormailzed(normalized)
		{}
		uint32_t GetComponentCount()const
		{
			switch (Type)
			{
			case ShaderDataType::None:		return 0;
			case ShaderDataType::Float:		return 1;
			case ShaderDataType::Float2:	return 2;
			case ShaderDataType::Float3:	return 3;
			case ShaderDataType::Float4:	return 4;
			case ShaderDataType::Int:		return 1;
			case ShaderDataType::Int2:		return 2;
			case ShaderDataType::Int3:		return 3;
			case ShaderDataType::Int4:		return 4;
			case ShaderDataType::Mat3x3:	return 9;
			case ShaderDataType::Mat4x4:	return 16;
			case ShaderDataType::Bool:		return 1;
			}
			DM_CORE_ASSERT(false, "{}", "Unknown ShaderDataType!");
			return 0;
		}
	};
	class DM_API BufferLayout
	{
	public:
		BufferLayout() = default;
		BufferLayout(const std::initializer_list<BufferElement>&Elements) :m_Elements(Elements)
		{
			UpdateElementsData();
		}
		inline uint32_t GetStride()const { return m_Stride; }
		inline const std::vector<BufferElement>& GetBufferElements()const { return m_Elements; }
		std::vector<BufferElement>::iterator begin()			{ return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end()				{ return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin()const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end()const	{ return m_Elements.end(); }
	private:
		void UpdateElementsData()
		{
			uint32_t offset = 0;
			m_Stride = 0;
			int location = 0;
			for (auto& element : m_Elements) 
			{ 
				element.Offset = offset;
				offset += element.Size_byte;
				m_Stride += element.Size_byte;
			}
		}
	private:
		std::vector<BufferElement>m_Elements;
		uint32_t m_Stride = 0;
	};


	class DM_API VertexBuffer
	{ 
	public:
		virtual ~VertexBuffer() = default;
		virtual void Bind() = 0;
		virtual void UnBind() = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;
		virtual void SetData(const void*data,uint32_t size) = 0;
		virtual BufferLayout GetLayout()const = 0;
		static SPtr<VertexBuffer>Create(uint32_t sizeByte);
		static SPtr<VertexBuffer>Create(float* vertices, uint32_t elementCount);
	};


	class DM_API IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;
		virtual void Bind() = 0;
		virtual void UnBind() = 0;
		virtual uint32_t GetCount()const = 0;
		static SPtr<IndexBuffer> Create(uint32_t* indices, uint32_t elementCount);
	};
}


