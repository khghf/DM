#include<DMPCH.h>
#include"Core/Render/VertexArray.h"
#include<Core/RHI/RHIDevice.h>
namespace DM
{
	SPtr<VertexArray> VertexArray::Create()
	{
		return RHI::RHIDevice::Get().CreateVertexArray();
	}
}