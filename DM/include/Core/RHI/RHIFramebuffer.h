#pragma once
#include"RHIResource.h"
namespace DM::RHI
{
	class RHIFramebuffer:public RHIResource
	{
	public:
		RHIFramebuffer()=default;
		~RHIFramebuffer()=default;
		virtual EResourceType GetAssetType() const { return EResourceType::Framebuffer; };

		virtual void Resize(const uint32_t& width, const uint32_t& height) = 0;

		uint32_t GetWidth()const { return m_Witdh; }
		uint32_t GetHeight()const { return m_Height; }
	protected:
		uint32_t m_Witdh;
		uint32_t m_Height;
	};
}