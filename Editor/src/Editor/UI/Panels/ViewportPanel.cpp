#include <DMPCH.h>
#include "Editor/UI/Panels/ViewportPanel.h"
#include <glm/gtc/type_ptr.hpp>
#include<Core/AssetManagent/AssetMgr.h>
#include <Framework/Component/Component.h>
#include "DM.h"
#include<Editor/Core/EditorContext.h>
#include <Editor/UI/EditorUI.h>
#include <Editor/UI/Widgets/DragItem.h>

namespace DM
{
	namespace
	{
		void HandleDragDrop(const DragItem& item)
		{
			if (!item.IsValid() || item.SourceFilePath.empty())
			{
				return;
			}

			switch (item.Type)
			{
			case EAssetType::World:
			{
				if (FileSystem::Exists(item.SourceFilePath))
				{
					SPtr<World> world = AssetMgr::LoadAsset<World>(item.SourceFilePath);
					if (world)
					{
						EditorContext::Get()->ChangeEditorWorld(world);
						LOG_CORE_INFO("{}", world->m_Name);
					}
				}
				break;
			}
			default:
				LOG_CORE_WARN("Unhandled drag & drop asset type: {}", item.SourceFilePath);
				break;
			}
		}
	}

	ViewportPanel::ViewportPanel()
	{
		m_ViewportRenderer = CreateUPtr<EditorViewportRenderer>();
	}

	void ViewportPanel::HandleEvent(Event* const e)
	{
		//m_CameraController.HandleEvent(e);
	}

	void ViewportPanel::UpdateViewPortData()
	{
		ImGuiStyle& style = EditorUI::GetStyle();
		m_ViewPort.bHovered = EditorUI::IsWindowHovered();
		m_ViewPort.bFocused = EditorUI::IsWindowFocused();

		Vector2 viewportPanelSize{};
		ImVec2 imvec = EditorUI::GetContentRegionAvail();

		viewportPanelSize.x = imvec.x;
		viewportPanelSize.y = imvec.y;
		m_ViewPort.Size = viewportPanelSize;

		if (m_ViewPort.bFocused && m_ViewPort.bHovered)
		{
			ImVec2 screenPos = EditorUI::GetCursorScreenPos();
			m_ViewPort.MinBound.x = screenPos.x;
			m_ViewPort.MinBound.y = screenPos.y - style.WindowPadding.y / 2.f;
			m_ViewPort.MaxBound = m_ViewPort.MinBound + m_ViewPort.Size;
			ImVec2 mousePos = EditorUI::GetMousePos();
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
		EditorUI::Begin("ViewPort", 0, ImGuiWindowFlags_NoTitleBar);
		{
			UpdateViewPortData();

			EditorUI::SetNextWindowPos(ImVec2(
				EditorUI::GetWindowPos().x + 10.0f,
				EditorUI::GetWindowPos().y + 10.0f
			));
			EditorUI::SetNextWindowSize(ImVec2(200.0f, 100.0f));

			EditorUI::Begin("##ViewportInfo",
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
				EditorUI::TextColored(ImVec4(1, 1, 1, 0.8f), "FPS: %.1f", EditorUI::GetIO().Framerate);
				EditorUI::TextColored(ImVec4(1, 1, 1, 0.8f), "Frame Time: %.3f ms", 1000.0f / EditorUI::GetIO().Framerate);
			}
			EditorUI::End();

			// 渲染引擎帧缓冲到视口(后端细节由渲染服务封装)
			ImVec2 imvec = EditorUI::GetContentRegionAvail();
			EditorUI::Image(m_ViewportRenderer->GetViewportTextureRef(), imvec, { 0,0 }, { 1,1 });

			if (EditorUI::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = EditorUI::AcceptDragDropPayload(DragItem::AssetPayloadType.c_str()))
				{
					HandleDragDrop(DragItem::FromPayload(payload));
				}
				EditorUI::EndDragDropTarget();
			}
		}
		EditorUI::End();
	}

	void ViewportPanel::RenderSetting()
	{
		EditorUI::Begin("Setting");
		{
			EditorUI::SeparatorText("ViewPortData");

			EditorUI::Text("MinBoundX:%f", m_ViewPort.MinBound.x);
			EditorUI::Text("MinBoundY:%f", m_ViewPort.MinBound.y);
			EditorUI::Text("MaxBoundX:%f", m_ViewPort.MaxBound.x);
			EditorUI::Text("MaxBoundY:%f", m_ViewPort.MaxBound.y);
			EditorUI::Text("ViewPortSizeX:%f", m_ViewPort.Size.x);
			EditorUI::Text("ViewPortSizeY:%f", m_ViewPort.Size.y);
			EditorUI::Text("MousePosLocal_X:%f", m_ViewPort.MousePosLocal.x);
			EditorUI::Text("MousePosLocal_Y:%f", m_ViewPort.MousePosLocal.y);
			EditorUI::Text("MousePosScreen_X:%f", m_ViewPort.MousePosScreen.x);
			EditorUI::Text("MousePosScreen_Y:%f", m_ViewPort.MousePosScreen.y);
			EditorUI::Text("MousePosLocalNarmalX:%f", m_ViewPort.MousePosLocalNarmal.x);
			EditorUI::Text("MousePosLocalNarmalY:%f", m_ViewPort.MousePosLocalNarmal.y);
		}
		EditorUI::End();
	}
}
