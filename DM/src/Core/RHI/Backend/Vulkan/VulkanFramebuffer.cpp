#include<Core/RHI/Backend/Vulkan/VulkanFramebuffer.h>
#include<Core/RHI/Backend/Vulkan/VulkanTypeMapping.h>
#include<Core/RHI/Backend/Vulkan/VulkanDevice.h>
#include<assert.h>
#include<vk_mem_alloc.h>
namespace DM::RHI
{
	VulkanFramebuffer::VulkanFramebuffer(VulkanDevice* device, const RHIFramebufferDesc& desc) 
		:m_Device(device), m_vkFramebuffer(VK_NULL_HANDLE), m_vkColorFormat(VK_FORMAT_UNDEFINED),m_Desc(desc)
	{
		m_Witdh = desc.Width;
		m_Height = desc.Height;
	}
	VulkanFramebuffer::VulkanFramebuffer(VulkanDevice* device, const RHIFramebufferDesc& desc, const VkRenderPass& renderPass):VulkanFramebuffer(device, desc)
	{
		m_vkRenderPass = renderPass;
		CreatevkFramebuffer();
	}
	
	VulkanFramebuffer::~VulkanFramebuffer()
	{
		for (const auto& in : m_Attachments)
		{
			DestroyAttachment(in);
		}
		DestroyAttachment(m_ResolveAttachment);
		vkDestroyFramebuffer(m_Device->GetvkDevice(), m_vkFramebuffer, nullptr);
	}

	void VulkanFramebuffer::CreatevkFramebuffer()
	{
		VkFramebufferCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.renderPass = m_vkRenderPass;
		info.width = m_Desc.Width;
		info.height = m_Desc.Height;
		// --- layers：层数 ---
		// 对于 2D 图像，layers = 1
		// 对于立方体贴图或纹理数组，可以 > 1
		// 立体渲染(VR)可能会用到多层
		info.layers = 1;

		CreateAttachments(m_Desc);

		std::vector<VkImageView>imageViews;

		if (m_ResolveAttachment.vkImage != VK_NULL_HANDLE)	imageViews.reserve(m_Attachments.size() + 1);
		else												imageViews.reserve(m_Attachments.size());

		for (const auto& attachment : m_Attachments)
		{
			imageViews.emplace_back(attachment.vkImageView);
		}

		if (m_ResolveAttachment.vkImage != VK_NULL_HANDLE)imageViews.emplace_back(m_ResolveAttachment.vkImageView);

		info.attachmentCount = imageViews.size();
		info.pAttachments = imageViews.data();


		VK_CHECK(vkCreateFramebuffer(m_Device->GetvkDevice(), &info, nullptr, &m_vkFramebuffer));
	}

	void VulkanFramebuffer::CreateAttachments(const RHIFramebufferDesc& desc)
	{
		const std::vector<AttachmentDesc>& attachmentDescs = desc.Attachments;

		VkSampleCountFlagBits sampleCount = ToVkSampleCount(desc.SampleMode);
		m_Attachments.resize(attachmentDescs.size());

		bool enableMultisample = sampleCount != VK_SAMPLE_COUNT_1_BIT;


		for (int i = 0; i < m_Attachments.size();++i)
		{
			const auto&attachmentDesc = attachmentDescs[i];
			ERHIAttachmentUsage usage = attachmentDesc.Usage;

			auto& image		= m_Attachments[i].vkImage;
			auto& imageView = m_Attachments[i].vkImageView;
			auto& memory	= m_Attachments[i].vmaAllocation;

			switch (usage)
			{
			case DM::RHI::ERHIAttachmentUsage::Unknown:
				break;
			case DM::RHI::ERHIAttachmentUsage::ColorTarget:
			{
				m_ColorIndex = i;
				VkFormat format = ToVkFormat(attachmentDesc.Format);
				m_vkColorFormat = format;

				if (enableMultisample)
				{
					m_Device->CreatevkImage(VK_IMAGE_TYPE_2D, desc.Width, desc.Height, format, sampleCount, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, memory);
				}
				else
				{
					m_Device->CreatevkImage(VK_IMAGE_TYPE_2D, desc.Width, desc.Height, format, sampleCount, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, memory);
				}
				m_Device->CreatevkImageView(image, VK_IMAGE_VIEW_TYPE_2D, format, VK_IMAGE_ASPECT_COLOR_BIT, imageView);
			}
				break;
			case DM::RHI::ERHIAttachmentUsage::ColorIntermediate:
				break;
			case DM::RHI::ERHIAttachmentUsage::ColorResolve:
				break;
			case DM::RHI::ERHIAttachmentUsage::DepthTarget:
			{
				VkFormat format = m_Device->FindDepthFormat();
				m_Device->CreatevkImage(VK_IMAGE_TYPE_2D, desc.Width, desc.Height, format, sampleCount, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, memory);
				m_Device->CreatevkImageView(image, VK_IMAGE_VIEW_TYPE_2D, format, VK_IMAGE_ASPECT_DEPTH_BIT, imageView);
			}
				break;
			case DM::RHI::ERHIAttachmentUsage::GBufferAlbedo:
				break;
			case DM::RHI::ERHIAttachmentUsage::GBufferNormal:
				break;
			case DM::RHI::ERHIAttachmentUsage::GBufferPosition:
				break;
			case DM::RHI::ERHIAttachmentUsage::GBufferMaterial:
				break;
			case DM::RHI::ERHIAttachmentUsage::ShadowMap:
				break;
			case DM::RHI::ERHIAttachmentUsage::SSAOMap:
				break;
			case DM::RHI::ERHIAttachmentUsage::BloomTarget:
				break;
			case DM::RHI::ERHIAttachmentUsage::Transient:
				break;
			case DM::RHI::ERHIAttachmentUsage::Persistent:
				break;
			case DM::RHI::ERHIAttachmentUsage::Present:
				break;
			case DM::RHI::ERHIAttachmentUsage::SwapchainBackBuffer:
				break;
			default:
				break;
			}
		}

		if (enableMultisample)
		{
			auto& image = m_ResolveAttachment.vkImage;
			auto& imageView = m_ResolveAttachment.vkImageView;
			auto& memory = m_ResolveAttachment.vmaAllocation;


			assert(m_vkColorFormat != VK_FORMAT_UNDEFINED && "[VulkanFramebuffer]Multisampling was used,but no color attachment was added");

			m_Device->CreatevkImage(
				VK_IMAGE_TYPE_2D, desc.Width, desc.Height, m_vkColorFormat, 
				VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, 
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT|VK_IMAGE_USAGE_SAMPLED_BIT, 
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, memory);
		
			m_Device->CreatevkImageView(image, VK_IMAGE_VIEW_TYPE_2D, m_vkColorFormat, VK_IMAGE_ASPECT_COLOR_BIT, imageView);
		}
	}
	void VulkanFramebuffer::DestroyAttachment(const Attachment& inAttachment)
	{
		vkDestroyImageView(m_Device->GetvkDevice(), inAttachment.vkImageView, nullptr);
		if (inAttachment.vmaAllocation)
			vmaDestroyImage(m_Device->GetVmaAllocator(), inAttachment.vkImage, inAttachment.vmaAllocation);
	}

	void VulkanFramebuffer::Resize(const uint32_t& width, const uint32_t& height)
	{
		if (m_Witdh == width && m_Height == height) return;

		m_Witdh = width;
		m_Height = height;
		m_Desc.Width = m_Witdh;
		m_Desc.Height = m_Height;

		// 使用延迟销毁：把旧的附件/framebuffer 移交给 VMA 帧队列，
		// 等待 GPU 完成当前帧的工作后自动释放，避免 vkDeviceWaitIdle 阻塞。
		VulkanDevice* device = m_Device;
		std::vector<Attachment> oldAttachments = std::move(m_Attachments);
		Attachment oldResolve = m_ResolveAttachment;
		VkFramebuffer oldFramebuffer = m_vkFramebuffer;
		m_Attachments.clear();
		m_ResolveAttachment = {};
		m_vkFramebuffer = VK_NULL_HANDLE;

		device->DeferFree([device, oldAttachments, oldResolve, oldFramebuffer]()
		{
			for (const auto& attachment : oldAttachments)
			{
				vkDestroyImageView(device->GetvkDevice(), attachment.vkImageView, nullptr);
				if (attachment.vmaAllocation)
					vmaDestroyImage(device->GetVmaAllocator(), attachment.vkImage, attachment.vmaAllocation);
			}
			vkDestroyImageView(device->GetvkDevice(), oldResolve.vkImageView, nullptr);
			if (oldResolve.vmaAllocation)
				vmaDestroyImage(device->GetVmaAllocator(), oldResolve.vkImage, oldResolve.vmaAllocation);
			vkDestroyFramebuffer(device->GetvkDevice(), oldFramebuffer, nullptr);
		});

		CreatevkFramebuffer();
	}
}