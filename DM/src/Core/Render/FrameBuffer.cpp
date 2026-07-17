#include "DMPCH.h"
#include <Core/Render/FrameBuffer.h>
#include<Core/RHI/RHIDevice.h>
namespace DM
{
	SPtr<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& spec)
	{
		return RHI::RHIDevice::Get().CreateFrameBuffer(spec);
	}

}