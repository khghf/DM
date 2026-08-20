#pragma once
#include "../../RHIRenderPass.h"
#include <vulkan/vulkan.h>
#include <unordered_map>
#include <vector>

namespace DM::RHI
{
	class VulkanDevice;
	class VulkanFramebuffer;

	class VulkanRenderPass : public RHIRenderPass
	{
		enum AttachmentType
		{
			Undefine,
			Color,
			Depth,
			Input,
			Resolve,
		};

		using AttachmentDescriptionSet = std::unordered_map<AttachmentType, std::vector<VkAttachmentDescription>>;
		using AttachmentReferenceSet = std::unordered_map<AttachmentType, std::vector<VkAttachmentReference>>;

	public:
		VulkanRenderPass(VulkanDevice* device, const RHIRenderPassDesc& desc);
		~VulkanRenderPass();

		virtual RHIFramebuffer* CreateFramebuffer(const RHIFramebufferDesc& desc)override;


		VkRenderPass			GetvkRenderPass() const { return m_vkRenderPass; }
		VkFormat				GetColorFormat() const { return m_vkColorFormat; }
		VkSampleCountFlagBits	GetvkSampleCount() const { return m_vkSampleCount; }

	private:
		std::pair<AttachmentDescriptionSet, AttachmentReferenceSet>CreatevkAttachmentDescriptionAndReference(const RHIRenderPassDesc& desc);

		void CreateRenderPass(const RHIRenderPassDesc& desc);
		void CreateSubpass(VkSubpassDescription& subpass,const AttachmentReferenceSet& referenceSet,bool bEnableMSAA);

	private:
		VulkanDevice*			m_Device;
		VkRenderPass            m_vkRenderPass;
		VkFormat                m_vkColorFormat;
		VkSampleCountFlagBits   m_vkSampleCount;
	};
}