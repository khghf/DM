#include <Core/RHI/Backend/Vulkan/VulkanRenderPass.h>
#include <Core/RHI/Backend/Vulkan/VulkanDevice.h>
#include <Core/RHI/Backend/Vulkan/VulkanFramebuffer.h>
#include <Core/RHI/Backend/Vulkan/VulkanTypeMapping.h>

namespace DM::RHI
{
	VulkanRenderPass::VulkanRenderPass(VulkanDevice* device, const RHIRenderPassDesc& desc)
		: m_Device(device)
		, m_vkRenderPass(VK_NULL_HANDLE)
		, m_vkColorFormat(ToVkFormat(desc.Format))
		, m_vkSampleCount(ToVkSampleCount(desc.SampleMode))
	{
		CreateRenderPass(desc);
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		if (m_vkRenderPass)vkDestroyRenderPass(m_Device->GetvkDevice(), m_vkRenderPass, nullptr);
	}

	RHIFramebuffer* VulkanRenderPass::CreateFramebuffer(const RHIFramebufferDesc& desc)
	{
		return new VulkanFramebuffer(m_Device,desc,m_vkRenderPass);
	}

	void VulkanRenderPass::CreateRenderPass(const RHIRenderPassDesc& desc)
	{
		bool bEnableMSAA = m_vkSampleCount != VK_SAMPLE_COUNT_1_BIT;

		auto descriptionAndReference = CreatevkAttachmentDescriptionAndReference(desc);
		auto& descriptionSet = descriptionAndReference.first;
		auto& referenceSet = descriptionAndReference.second;

		// 收集所有附件描述
		std::vector<VkAttachmentDescription> descriptions;
		for (auto& [type, descs] : descriptionSet)
		{
			descriptions.insert(descriptions.end(), descs.begin(), descs.end());
		}

		//创建子通道描述
		VkSubpassDescription subpass{};
		CreateSubpass(subpass, referenceSet, bEnableMSAA);
		
		//创建子通道依赖
		VkSubpassDependency dependency{};

		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;//依赖的来源，VK_SUBPASS_EXTERNAL表示为外部操作
		//srcStageMask：说明来源在那些阶段发生过操作
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		if (desc.EnableDepth)dependency.srcStageMask |= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		//srcStageMask：说明来源在每个阶段执行的操作
		dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		if (desc.EnableDepth)dependency.srcAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;



		dependency.dstSubpass = 0;
		//dstStageMask：说明目标要等待的阶段
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		if (desc.EnableDepth)dependency.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		//dstAccessMask：说明目标等待到指定阶段后要执行的操作
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		if (desc.EnableDepth)dependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


		std::vector<VkSubpassDescription>subpasses{ subpass };
		std::vector<VkSubpassDependency>dependencies{ dependency };

		VkRenderPassCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		info.attachmentCount = static_cast<uint32_t>(descriptions.size());
		info.pAttachments = descriptions.data();
		info.subpassCount = subpasses.size();
		info.pSubpasses = subpasses.data();
		info.dependencyCount = dependencies.size();
		info.pDependencies = dependencies.data();

		VK_CHECK(vkCreateRenderPass(m_Device->GetvkDevice(), &info, nullptr, &m_vkRenderPass));
	}

	std::pair<VulkanRenderPass::AttachmentDescriptionSet, VulkanRenderPass::AttachmentReferenceSet>VulkanRenderPass::CreatevkAttachmentDescriptionAndReference(const RHIRenderPassDesc& desc)
	{
		std::pair<AttachmentDescriptionSet, AttachmentReferenceSet> result;
		auto& descriptionSet = result.first;
		auto& referenceSet = result.second;

		bool enableMultisample = m_vkSampleCount != VK_SAMPLE_COUNT_1_BIT;

		for (size_t i = 0; i < desc.Attachments.size(); ++i)
		{
			const auto& attachment = desc.Attachments[i];
			//附件的完整定义
			VkAttachmentDescription description{};
			//子通道对附件的引用(指向哪个+子通道内布局)
			VkAttachmentReference reference{};
			reference.attachment = static_cast<uint32_t>(i);

			AttachmentType attachmentType = AttachmentType::Undefine;
			ERHIAttachmentUsage usage = attachment.Usage;

			switch (usage)
			{
			case ERHIAttachmentUsage::ColorTarget:
			{
				attachmentType = AttachmentType::Color;
				description.format = m_vkColorFormat;
				description.samples = m_vkSampleCount;
				description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

				if (enableMultisample)
				{
					description.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
					description.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				}
				else
				{
					description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
					//使用离屏渲染所以最终结果的布局都要作为拷贝源
					description.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				}

				reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				break;
			}

			case ERHIAttachmentUsage::ColorIntermediate:
			{
				attachmentType = AttachmentType::Color;
				description.format = ToVkFormat(attachment.Format);
				description.samples = m_vkSampleCount;
				description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				description.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				break;
			}

			case ERHIAttachmentUsage::ColorResolve:
			{
				attachmentType = AttachmentType::Resolve;
				description.format = m_vkColorFormat;
				description.samples = VK_SAMPLE_COUNT_1_BIT;
				description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
				description.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				break;
			}

			case ERHIAttachmentUsage::DepthTarget:
			{
				attachmentType = AttachmentType::Depth;
				description.format = m_Device->FindDepthFormat();
				description.samples = m_vkSampleCount;
				description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				description.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

				reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				break;
			}

			case ERHIAttachmentUsage::GBufferAlbedo:
			case ERHIAttachmentUsage::GBufferNormal:
			case ERHIAttachmentUsage::GBufferPosition:
			case ERHIAttachmentUsage::GBufferMaterial:
			{
				attachmentType = AttachmentType::Color;
				description.format = ToVkFormat(attachment.Format);
				description.samples = m_vkSampleCount;
				description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				description.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

				reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				break;
			}

			default:
				assert(false && "Unsupported AttachmentUsage");
				break;
			}

			descriptionSet[attachmentType].emplace_back(description);
			referenceSet[attachmentType].emplace_back(reference);
		}

		if (enableMultisample)
		{
			// 检查是否已经有 Resolve 附件
			auto resolveIt = referenceSet.find(AttachmentType::Resolve);
			if (resolveIt == referenceSet.end() || resolveIt->second.empty())
			{
				// 自动添加解析附件
				VkAttachmentDescription resolveDesc{};
				resolveDesc.format = m_vkColorFormat;
				resolveDesc.samples = VK_SAMPLE_COUNT_1_BIT;
				resolveDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				resolveDesc.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

				resolveDesc.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				resolveDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

				resolveDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				resolveDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

				VkAttachmentReference resolveRef{};
				resolveRef.attachment = static_cast<uint32_t>(desc.Attachments.size());
				resolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				descriptionSet[AttachmentType::Resolve].emplace_back(resolveDesc);
				referenceSet[AttachmentType::Resolve].emplace_back(resolveRef);
			}
		}
		return result;
	}


	void VulkanRenderPass::CreateSubpass(VkSubpassDescription& subpass,const AttachmentReferenceSet& referenceSet,bool bEnableMSAA)
	{
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

		// 颜色附件
		auto colorIt = referenceSet.find(AttachmentType::Color);
		if (colorIt != referenceSet.end() && !colorIt->second.empty())
		{
			subpass.colorAttachmentCount = colorIt->second.size();
			subpass.pColorAttachments = colorIt->second.data();
		}

		// 解析附件
		if (bEnableMSAA)
		{
			auto resolveIt = referenceSet.find(AttachmentType::Resolve);
			if (resolveIt != referenceSet.end() && !resolveIt->second.empty())
			{
				subpass.pResolveAttachments = resolveIt->second.data();
			}
		}

		// 深度附件
		auto depthIt = referenceSet.find(AttachmentType::Depth);
		if (depthIt != referenceSet.end() && !depthIt->second.empty())
		{
			subpass.pDepthStencilAttachment = &depthIt->second[0];
		}

		// 输入附件
		auto inputIt = referenceSet.find(AttachmentType::Input);
		if (inputIt != referenceSet.end() && !inputIt->second.empty())
		{
			subpass.inputAttachmentCount = static_cast<uint32_t>(inputIt->second.size());
			subpass.pInputAttachments = inputIt->second.data();
		}
	}

	
	
}