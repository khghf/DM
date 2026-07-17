#include<DMPCH.h>
#include "Panel/SceneHierarchyPanel.h"
#include <Imgui.h>
#include <Imgui_internal.h>
#include <Framework/Entity.h>
#include<Framework/Component/Component.h>
#include<glm/gtc/type_ptr.hpp>
namespace DM
{
	static void DrawVec3Control(const std::string& label, Vector3& val, float resetVal = 0.f, float columnWidth = 100.f)
	{
		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		float lineHight = GImGui->Font->LegacySize + GImGui->Style.FramePadding.y * 2.f;
		ImVec2 buttonSize = { lineHight + 3,lineHight };
		float baseColor = 0.8f;
		ImGuiIO& io = ImGui::GetIO();
		ImFont* defaultfont = io.FontDefault;
		io.FontDefault = io.Fonts->Fonts[1];
		float DragSpeed = 0.001;
		////////////X
		ImGui::PushStyleColor(ImGuiCol_Button, { baseColor,0.f,0.f,1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1.f,0.f,0.f,1.f });
		ImGui::PushStyleColor(ImGuiCol_Button, { baseColor,0.f,0.f,1.f });
		if (ImGui::Button("X", buttonSize))val.x = resetVal;
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##X", &val.x, DragSpeed);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		////////////Y
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.f,baseColor,0.f,1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.f,1.f,0.f,1.f });
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.f,baseColor,0.f,1.f });
		if (ImGui::Button("Y", buttonSize))val.y = resetVal;
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Y", &val.y, DragSpeed);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		////////////Z
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.f,0.f,baseColor,1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.f,0.f,1.f,1.f });
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.f,0.f,baseColor,1.f });
		if (ImGui::Button("Z", buttonSize))val.z = resetVal;
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Z", &val.z, DragSpeed);
		ImGui::PopItemWidth();
		io.FontDefault = defaultfont;
		ImGui::Columns(1);
		ImGui::PopID();
	}
	void SceneHierarchyPanel::Render()
	{
		ImGui::Begin("SceneHierarchyPanel");
		{
			if (m_Context)
			{
				for (const auto& entity : m_Context->m_Registry.view<entt::entity>())
				{
					if (m_Context->m_Registry.valid(entity))
					{
						Entity en{ entity,m_Context.get() };
						DrawEntityNode(en);
					}
				}

				if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())m_SelectedEntity = {};

				if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
				{
					static int createcount = 10;

					ImGui::DragInt("Count", &createcount);
					if (ImGui::MenuItem("Create Entity"))
					{
						for (int i = 0; i < createcount; i++)
						{
							m_Context->CreateEntity();
						}
					}
					ImGui::EndPopup();
				}
			}
			
		}
		ImGui::End();
		DrawComponents(m_SelectedEntity);
	}

	void SceneHierarchyPanel::DrawEntityNode(const Entity& entity)
	{
		ImGuiTreeNodeFlags flag =ImGuiTreeNodeFlags_OpenOnArrow| ImGuiTreeNodeFlags_SpanAvailWidth| ImGuiTreeNodeFlags_FramePadding|((m_SelectedEntity==entity?ImGuiTreeNodeFlags_Selected:0));
		const auto& Tag = entity.GetComponent<TagComponent>()->Tag;
		bool opened = ImGui::TreeNodeEx((void*)(entity.m_Handle), flag, Tag.c_str());
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))m_Context->DestroyEntity(entity);
			ImGui::EndPopup();
		}
		if(ImGui::IsItemClicked())
		{
			m_SelectedEntity = entity;
		}
		
		if (opened)
		{
			ImGui::TreePop();
		}

	}

	void SceneHierarchyPanel::DrawComponents(const Entity& entity)
	{
		ImGui::Begin("Property");
		if (entity)
		{
			if (entity.HasComponent<TagComponent>())
			{
				auto label = entity.GetComponent<TagComponent>()->Tag;
				char buffer[128];
				memset(buffer, 0, 128);
				strcpy_s(buffer, sizeof(buffer), label.c_str());
				ImGuiInputTextFlags flag = ImGuiInputTextFlags_EnterReturnsTrue;
				if (ImGui::InputText(label.c_str(), buffer,128, flag))
				{
					label = buffer;
				}
			}
			if (entity.HasComponent<TransformComponent>())
			{
				auto transform = entity.GetComponent<TransformComponent>();
				DrawVec3Control("Location", transform->Location);
				DrawVec3Control("Rotation", transform->Rotation);
				DrawVec3Control("Scale", transform->Scale,1.f);
			}
			if (entity.HasComponent<SpriteComponent>())
			{
				auto sprite = entity.GetComponent<SpriteComponent>();
				ImGui::ColorEdit4("Color",glm::value_ptr(sprite->Color));
			}
		}
		ImGui::End();
	}
	
}