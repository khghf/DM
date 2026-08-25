#include <DMPCH.h>
#include "Editor/UI/Viewport/EditorViewportRenderer.h"
#include <Engine.h>
#ifdef ENABLE_VULKAN_API
#include<Core/RHI/Backend/Vulkan/VulkanFramebuffer.h>
#include<Core/RHI/Backend/Vulkan/VulkanCommandList.h>
#include<Core/RHI/Backend/Vulkan/VulkanDevice.h>
#include<backends/imgui_impl_vulkan.h>
#endif

namespace DM
{
#ifdef ENABLE_VULKAN_API
	static VkDescriptorSet	s_FramebufferSet;
	static VkImageView		s_ViewportImageView;
#endif

	EditorViewportRenderer::EditorViewportRenderer()
	{
	}

	EditorViewportRenderer::~EditorViewportRenderer()
	{
#ifdef ENABLE_VULKAN_API
		if (s_FramebufferSet != VK_NULL_HANDLE)
		{
			ImGui_ImplVulkan_RemoveTexture(s_FramebufferSet);
			s_FramebufferSet = VK_NULL_HANDLE;
		}
		s_ViewportImageView = VK_NULL_HANDLE;
#endif
	}

	void EditorViewportRenderer::UpdateTextureIfNeeded()
	{
#ifdef ENABLE_VULKAN_API
		auto renderer = Engine::GetRenderer();
		auto device = static_cast<RHI::VulkanDevice*>(RHI::GetDevice());
		RHI::VulkanFramebuffer* framebuffer = static_cast<RHI::VulkanFramebuffer*>(renderer->GetFramebuffer());
		RHI::VulkanCommandList* cmdList = static_cast<RHI::VulkanCommandList*>(renderer->GetCommandList());

		auto colorAttachment = framebuffer->GetColorAttachment();
		device->TransitionImageLayout(*cmdList->GetIdleCommandBuffer(),colorAttachment.vkImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		if (s_FramebufferSet == VK_NULL_HANDLE || s_ViewportImageView != colorAttachment.vkImageView)
		{
			ImGui_ImplVulkan_RemoveTexture(s_FramebufferSet);
			s_FramebufferSet = ImGui_ImplVulkan_AddTexture(
				colorAttachment.vkImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			s_ViewportImageView = colorAttachment.vkImageView;
			m_TextureRef = (ImTextureID)(s_FramebufferSet);
		}
#endif
	}

	ImTextureRef EditorViewportRenderer::GetViewportTextureRef()
	{
		UpdateTextureIfNeeded();
		return m_TextureRef;
	}
}
