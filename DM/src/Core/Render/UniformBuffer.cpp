#include "DMPCH.h"
#include "Core/Render/UniformBuffer.h"
#include<Core/RHI/RHIDevice.h>
namespace DM
{
	SPtr<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		return RHI::RHIDevice::Get().CreateUniformBuffer(size, binding);
	}
}