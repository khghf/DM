#pragma once
#include"../../RHIFramebuffer.h"
#include<vulkan/vulkan.h>
namespace DM::RHI
{
	class VulkanDevice;
	class VulkanFramebuffer:public RHIFramebuffer
	{
	public:
		struct Attachment
		{
			VkImage			vkImage;
			VkImageView		vkImageView;
			VkDeviceMemory	vkMemory;
		};

		VulkanFramebuffer(VulkanDevice* device, const RHIFramebufferDesc& desc);
		VulkanFramebuffer(VulkanDevice*device,const RHIFramebufferDesc&desc,const VkRenderPass&renderPass);
		~VulkanFramebuffer();

	private:
		void CreatevkFramebuffer();
		void CreateAttachments(const RHIFramebufferDesc& desc);
		void DestroyAttachment(const Attachment& inAttachment);
	public:
		VkFramebuffer	GetvkFramebuffer()const { return m_vkFramebuffer; }

		Attachment		GetColorAttachment()const 
		{ 
			if (m_ResolveAttachment.vkImage == VK_NULL_HANDLE)return m_Attachments[m_ColorIndex]; 
			else return m_ResolveAttachment; 
		}

		virtual void	Resize(const uint32_t& width, const uint32_t& height)override;
	private:
		VulkanDevice*			m_Device;
		VkRenderPass			m_vkRenderPass;
		VkFramebuffer			m_vkFramebuffer;

		std::vector<Attachment>	m_Attachments;
		Attachment				m_ResolveAttachment;//使用多重采样时需要一个额外的解析附件

		RHIFramebufferDesc		m_Desc;

		VkFormat				m_vkColorFormat;
		int8_t					m_ColorIndex=-1;
	};
}