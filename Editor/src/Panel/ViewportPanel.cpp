#include <DMPCH.h>
#include "Panel/ViewportPanel.h"
#include <glm/gtc/type_ptr.hpp>
#include<Core/AssetManagent/AssetMgr.h>
#include <Framework/Component/Component.h>
#include "DM.h"
#ifdef ENABLE_VULKAN_API
#include<Core/RHI/Backend/Vulkan/VulkanFramebuffer.h>
#include<Core/RHI/Backend/Vulkan/VulkanCommandList.h>
#include<Core/RHI/Backend/Vulkan/VulkanDevice.h>
#include<backends/imgui_impl_vulkan.h>


static VkDescriptorSet	framebufferSet;
static VkImageView		viewPortImageView;

#endif // ENABLE_VULKAN_API
#include<Editor.h>

namespace DM
{
	ViewportPanel::ViewportPanel()
	{

	}

	void ViewportPanel::HandleEvent(Event* const e)
	{
		//m_CameraController.HandleEvent(e);
	}

	void ViewportPanel::UpdataViewPortData()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		m_ViewPort.bHovered = ImGui::IsWindowHovered();
		m_ViewPort.bFocused = ImGui::IsWindowFocused();

		Vector2 viewportPanelSize{};
		ImVec2 imvec = ImGui::GetContentRegionAvail();

		viewportPanelSize.x = imvec.x;
		viewportPanelSize.y = imvec.y;

		if (m_ViewPort.bFocused && m_ViewPort.bHovered)
		{
			ImVec2 screenPos = ImGui::GetCursorScreenPos();
			m_ViewPort.MinBound.x = screenPos.x;
			m_ViewPort.MinBound.y = screenPos.y - style.WindowPadding.y / 2.f;
			m_ViewPort.MaxBound = m_ViewPort.MinBound + m_ViewPort.Size;
			ImVec2 mousePos = ImGui::GetMousePos();
			m_ViewPort.MousePosScreen.x = mousePos.x;
			m_ViewPort.MousePosScreen.y = mousePos.y;
			m_ViewPort.MousePosLocal.x = mousePos.x - m_ViewPort.MinBound.x;
			m_ViewPort.MousePosLocal.y = m_ViewPort.MinBound.y - mousePos.y;
			m_ViewPort.MousePosLocalNarmal = m_ViewPort.MousePosLocal / m_ViewPort.Size;
		}
	}

	void ViewportPanel::Render()
	{
		RenderViewPort();
		RenderSetting();
	}

	void ViewportPanel::RenderViewPort()
	{
		ImGui::Begin("ViewPort", 0, ImGuiWindowFlags_NoTitleBar);
		{
			UpdataViewPortData();

			ImGui::SetNextWindowPos(ImVec2(
				ImGui::GetWindowPos().x + 10.0f,
				ImGui::GetWindowPos().y + 10.0f
			));
			ImGui::SetNextWindowSize(ImVec2(200.0f, 100.0f));

			ImGui::Begin("##ViewportInfo",
				nullptr,
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoScrollbar |
				ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_NoInputs |           // 不拦截鼠标
				ImGuiWindowFlags_NoBackground |       // 透明背景！
				ImGuiWindowFlags_NoDecoration);       // 无边框
			{
				ImGui::TextColored(ImVec4(1, 1, 1, 0.8f), "FPS: %.1f", ImGui::GetIO().Framerate);
				ImGui::TextColored(ImVec4(1, 1, 1, 0.8f), "Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
			}
			ImGui::End();



#ifdef  ENABLE_VULKAN_API
			auto renderer = Engine::GetRenderer();
			auto device = static_cast<RHI::VulkanDevice*>(RHI::GetDevice());
			RHI::VulkanFramebuffer* framebuffer = static_cast<RHI::VulkanFramebuffer*>(renderer->GetFramebuffer());
			RHI::VulkanCommandList* cmdList = static_cast<RHI::VulkanCommandList*>(renderer->GetCommandList());

			auto colorAttachment = framebuffer->GetColorAttachment();
			device->TransitionImageLayout(*cmdList->GetIdleCommandBuffer(),colorAttachment.vkImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			
			if (framebufferSet == VK_NULL_HANDLE|| viewPortImageView!=colorAttachment.vkImageView)
			{
				ImGui_ImplVulkan_RemoveTexture(framebufferSet);
				framebufferSet = ImGui_ImplVulkan_AddTexture(colorAttachment.vkImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
				viewPortImageView = colorAttachment.vkImageView;
				m_ViewPortTextureRef = (ImTextureID)(framebufferSet);
			}

			ImVec2 imvec = ImGui::GetContentRegionAvail();
			ImGui::Image(m_ViewPortTextureRef, imvec, { 0,0 }, { 1,1 });
#endif //  ENABLE_VULKAN_API


			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_DRAG_ITEM"))
				{
					std::string path= static_cast<const char*>(payload->Data);

					if (FileSystem::Exists(path))
					{

						if (FileSystem::GetExtension(path) == ".world")
						{

						}
						SPtr<World> world = AssetMgr::LoadAsset<World>(path);

						Editor::Get()->ChangeEditorWorld(world);

						LOG_CORE_INFO("{}", world->m_Name);
					}
				}
				ImGui::EndDragDropTarget();
			}
		}
		ImGui::End();
	}

	void ViewportPanel::RenderSetting()
	{
		ImGui::Begin("Setting");
		{
			ImGui::SeparatorText("ViewPortData");

			ImGui::Text("MinBoundX:%f", m_ViewPort.MinBound.x);
			ImGui::Text("MinBoundY:%f", m_ViewPort.MinBound.y);
			ImGui::Text("MaxBoundX:%f", m_ViewPort.MaxBound.x);
			ImGui::Text("MaxBoundY:%f", m_ViewPort.MaxBound.y);
			ImGui::Text("ViewPortSizeX:%f", m_ViewPort.Size.x);
			ImGui::Text("ViewPortSizeY:%f", m_ViewPort.Size.y);
			ImGui::Text("MousePosLocal_X:%f", m_ViewPort.MousePosLocal.x);
			ImGui::Text("MousePosLocal_Y:%f", m_ViewPort.MousePosLocal.y);
			ImGui::Text("MousePosScreen_X:%f", m_ViewPort.MousePosScreen.x);
			ImGui::Text("MousePosScreen_Y:%f", m_ViewPort.MousePosScreen.y);
			ImGui::Text("MousePosLocalNarmalX:%f", m_ViewPort.MousePosLocalNarmal.x);
			ImGui::Text("MousePosLocalNarmalY:%f", m_ViewPort.MousePosLocalNarmal.y);
		}
		ImGui::End();
	}
}
