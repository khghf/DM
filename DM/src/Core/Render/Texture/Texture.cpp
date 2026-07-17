#include<DMPCH.h>
#include "Core/Render/Texture/Texture.h"
#include<Core/RHI/RHIDevice.h>
namespace DM
{
	SPtr<Texture2D> Texture2D::Create( const std::string_view& path)
	{
		return RHI::RHIDevice::Get().CreateTexture2D(path);
	}
	SPtr<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		return RHI::RHIDevice::Get().CreateTexture2D(width, height);
	}
}

