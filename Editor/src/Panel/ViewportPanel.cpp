#include <DMPCH.h>
#include "Panel/ViewportPanel.h"
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <Core/Render/Renderer/Renderer2D.h>
#include <Core/Render/Texture/Texture.h>
#include <Framework/Component/Component.h>
#include "DM.h"

namespace DM
{
	ViewportPanel::ViewportPanel(const SPtr<World>& context, float windowWidth, float windowHeight)
	{
		SetContext(context);
		m_CameraController = CameraController(windowWidth / windowHeight, ECameraType::Ortho);

		FrameBufferSpecification spec;
		spec.Attachments = { FramebufferTextureFormat::Depth,
							 FramebufferTextureFormat::RGBA8,
							 FramebufferTextureFormat::RED_Int };
		spec.Width = (uint32_t)windowWidth;
		spec.Height = (uint32_t)windowHeight;

		m_ViewportFramebuffer = FrameBuffer::Create(spec);
	}

	void ViewportPanel::OnUpdate(float DeltaTime)
	{
		// 在渲染到 FBO 之前应用待处理的视口 resize，避免 FBO 重建导致的闪烁
		if (m_PendingViewportSize.x > 0.f || m_PendingViewportSize.y > 0.f)
		{
			if (m_ViewPort.Size != m_PendingViewportSize)
			{
				m_ViewPort.Size = m_PendingViewportSize;
				m_ViewportFramebuffer->Resize((uint32_t)m_ViewPort.Size.x, (uint32_t)m_ViewPort.Size.y);
				m_CameraController.SetProjection(m_ViewPort.Size.x / m_ViewPort.Size.y);
			}
			m_PendingViewportSize = { 0.f, 0.f };
		}

		m_ViewportFramebuffer->Bind();
		DM::RenderCommand::Clear();
		m_ViewportFramebuffer->ClearColorAttachment(1, -2);
		if (m_Context)
		{
			const auto& group = m_Context->m_Registry.group<SpriteComponent, TransformComponent>();
			if (m_ViewPort.bHovered) m_CameraController.OnUpdate(DeltaTime);
			DM::Renderer2D::BeginScene(m_CameraController.GetCamera());
			for (auto& entity : group)
			{
				const auto& [sprite, transform] = group.get<SpriteComponent, TransformComponent>(entity);
				DM::Renderer2D::DrawQuad(sprite, transform);
			}
			DM::Renderer2D::EndScene();
		}

		m_ViewportFramebuffer->UnBind();
	}

	void ViewportPanel::OnEvent(Event* const e)
	{
		m_CameraController.OnEvent(e);
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
			ImGuiStyle& style = ImGui::GetStyle();
			m_ViewPort.bHovered = ImGui::IsWindowHovered();
			m_ViewPort.bFocused = ImGui::IsWindowFocused();
			Vector2 viewportPanelSize;
			ImVec2 imvec = ImGui::GetContentRegionAvail();

			viewportPanelSize.x = imvec.x;
			viewportPanelSize.y = imvec.y;
			if (m_ViewPort.Size != viewportPanelSize)
			{
				// 不在此处立即 resize FBO，而是延迟到下一帧的 OnUpdate 中处理
				// 避免在 ImGui Render 阶段销毁并重建 FBO 造成的黑闪
				m_PendingViewportSize = viewportPanelSize;
			}
			uint32_t id = m_ViewportFramebuffer->GetColorAttachmentId();
			ImTextureRef textureRef((void*)id);
			ImGui::Image(textureRef, imvec, { 0,1 }, { 1,0 });
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
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_DRAG_ITEM"))
				{
					const wchar_t* data = static_cast<const wchar_t*>(payload->Data);
					if (OnOpenScene)
						OnOpenScene(data);
				}
				ImGui::EndDragDropTarget();
			}
		}
		ImGui::End();//ViewPort
	}

	void ViewportPanel::RenderSetting()
	{
		ImGui::Begin("Setting");
		{
			ImGui::SeparatorText("Render");
			ImGui::Text("DrawCall:%d", Renderer2D::GetStats().DrawCall);
			ImGui::Text("QuadCount:%d", Renderer2D::GetStats().QuadCount);
			ImGui::Text("VertexCount:%d", Renderer2D::GetStats().GetVertexCount());
			ImGui::Text("IndexCount:%d", Renderer2D::GetStats().GetIndexCount());
			ImGui::SeparatorText("CameraController");
			ImGui::SliderFloat("MoveSensitivity", &m_CameraController.m_MoveSensitivity, 1.f, 100.f);
			ImGui::SliderFloat("ZoomSensitivity", &m_CameraController.m_ZoomSensitivity, 1.f, 100.f);
			ImGui::SeparatorText("ViewPort");

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
		ImGui::End();//Setting
	}
}
