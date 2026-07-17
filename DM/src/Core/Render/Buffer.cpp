#include "DMPCH.h"
#include<Core/Render/Buffer.h>
#include<Core/RHI/RHIDevice.h>
namespace DM
{
	SPtr<VertexBuffer> VertexBuffer::Create(uint32_t sizeByte)
	{
		return RHI::RHIDevice::Get().CreateVertexBuffer(sizeByte);
	}
	SPtr<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t elementCount)
	{
		return RHI::RHIDevice::Get().CreateVertexBuffer(vertices, elementCount);
	}
	SPtr<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t elementCount)
	{
		return RHI::RHIDevice::Get().CreateIndexBuffer(indices, elementCount);
	}
}