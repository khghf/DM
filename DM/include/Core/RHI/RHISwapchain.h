#pragma once
#include"Core/RHI/RHIResource.h"

namespace DM::RHI
{
	class RHIFramebuffer;
	class DM_API RHISwapchain : public RHIResource
	{
	public:
		~RHISwapchain() override = default;

		EResourceType		GetAssetType() const override { return EResourceType::Swapchain; }
		virtual void		GetExtent(uint32_t& width, uint32_t& height) const = 0;
		virtual uint32_t	GetCurrentImageIndex() const = 0;
		virtual uint32_t	GetImageCount()const = 0;
		virtual ERHIFormat	GetFormat()const = 0;


		virtual uint32_t	AcquireNextImage() = 0;
		virtual void		Present() = 0;

		void SetPresentTaregt(RHIFramebuffer* framebuffer) { m_PresentTarget= framebuffer; }
	protected:
		RHISwapchain() = default;
		RHIFramebuffer* m_PresentTarget{};
	};

} // namespace DM::RHI
