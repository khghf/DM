#pragma once
#include"Core/RHI/RHIResource.h"

namespace DM::RHI
{
	class RHIFramebuffer;
	/// <summary>
	/// [笔记]交换链。在vulkan中交换链用于将图像结果呈现于屏幕上，交换链自带若干图像用于呈现结果
	/// </summary>
	class DM_API RHISwapchain : public RHIResource
	{
	public:
		~RHISwapchain() override = default;

		EResourceType		GetResourceType() const override { return EResourceType::Swapchain; }
		virtual void		GetExtent(uint32_t& width, uint32_t& height) const = 0;
		virtual uint32_t	GetCurrentImageIndex() const = 0;
		virtual uint32_t	GetImageCount()const = 0;
		virtual EFormat	GetFormat()const = 0;


		virtual uint32_t	AcquireNextImage() = 0;
		virtual void		Present() = 0;

		void SetPresentTaregt(RHIFramebuffer* framebuffer) { m_PresentTarget= framebuffer; }
	protected:
		RHISwapchain() = default;
		RHIFramebuffer* m_PresentTarget{};
	};

} // namespace DM::RHI
