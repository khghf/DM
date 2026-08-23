#include <DMPCH.h>
#include "Panel/SceneHierarchyPanel.h"
#include <Imgui.h>
#include <Imgui_internal.h>
#include <Framework/Base/Entity.h>
#include <Framework/Component/Component.h>
#include <Framework/Component/TransformComponent.h>
#include <Framework/Component/SpriteComponent.h>
#include <glm/gtc/type_ptr.hpp>

namespace DM
{
	// ============================================================
	// Vec3 控件
	// ============================================================
	static void DrawVec3Control(const std::string& label, Vector3& val,
		float resetVal = 0.f, float columnWidth = 100.f)
	{
		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

		float lineHeight = GImGui->Font->LegacySize + GImGui->Style.FramePadding.y * 2.f;
		ImVec2 buttonSize = { lineHeight + 3.f, lineHeight };
		float baseColor = 0.8f;

		ImGuiIO& io = ImGui::GetIO();
		ImFont* defaultFont = io.FontDefault;
		io.FontDefault = io.Fonts->Fonts[1];
		float dragSpeed = 0.001f;

		// X
		ImGui::PushStyleColor(ImGuiCol_Button, { baseColor, 0.f, 0.f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1.f, 0.f, 0.f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { baseColor, 0.f, 0.f, 1.f });
		if (ImGui::Button("X", buttonSize)) val.x = resetVal;
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##X", &val.x, dragSpeed);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// Y
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.f, baseColor, 0.f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.f, 1.f, 0.f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.f, baseColor, 0.f, 1.f });
		if (ImGui::Button("Y", buttonSize)) val.y = resetVal;
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Y", &val.y, dragSpeed);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// Z
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.f, 0.f, baseColor, 1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.f, 0.f, 1.f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.f, 0.f, baseColor, 1.f });
		if (ImGui::Button("Z", buttonSize)) val.z = resetVal;
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Z", &val.z, dragSpeed);
		ImGui::PopItemWidth();

		io.FontDefault = defaultFont;
		ImGui::Columns(1);
		ImGui::PopID();
	}

	// ============================================================
	// 渲染主面板
	// ============================================================
	void SceneHierarchyPanel::Render()
	{
		ImGui::Begin("Scene Hierarchy");
		{
			// 搜索过滤
			ImGui::InputText("Filter", m_Filter, sizeof(m_Filter));
			ImGui::Separator();

			// 重建缓存
			RebuildCacheIfNeeded();

			// 获取过滤后的列表
			const auto& list = GetFilteredEntities();

			// 虚拟列表：只渲染可见的
			ImGuiListClipper clipper;
			clipper.Begin(static_cast<int>(list.size()));

			while (clipper.Step())
			{
				for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
				{
					DrawEntityNode(list[i]);
				}
			}
			clipper.End();

			// 空白区域点击取消选中
			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			{
				m_SelectedEntity = {};
			}

			// 右键菜单：创建实体
			if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
			{
				static int createCount = 10;
				ImGui::DragInt("Count", &createCount, 1.f, 1, 100);

				if (ImGui::MenuItem("Create Entity"))
				{
					for (int i = 0; i < createCount; i++)
					{
						m_Context->CreateEntity();
					}
					m_CacheDirty = true;
				}
				ImGui::EndPopup();
			}
		}
		ImGui::End();

		// 属性面板
		DrawComponents(m_SelectedEntity);
	}

	// ============================================================
	// 重建缓存
	// ============================================================
	void SceneHierarchyPanel::RebuildCacheIfNeeded()
	{
		if (!m_CacheDirty) return;

		m_CachedEntities.clear();

		if (!m_Context) return;

		for (auto entity : m_Context->m_Registry.view<entt::entity>())
		{
			if (!m_Context->m_Registry.valid(entity)) continue;

			Entity en{ entity, m_Context.get() };
			auto* tag = en.GetComponent<TagComponent>();

			CachedEntity cached;
			cached.Handle = entity;
			cached.Tag = tag ? tag->Tag : "Entity";

			m_CachedEntities.push_back(std::move(cached));
		}

		m_CacheDirty = false;
	}

	// ============================================================
	// 获取过滤后的实体
	// ============================================================
	const std::vector<SceneHierarchyPanel::CachedEntity>&
		SceneHierarchyPanel::GetFilteredEntities()
	{
		if (m_Filter[0] == '\0')
		{
			return m_CachedEntities;
		}

		m_FilteredEntities.clear();
		for (const auto& entity : m_CachedEntities)
		{
			if (entity.Tag.find(m_Filter) != std::string::npos)
			{
				m_FilteredEntities.push_back(entity);
			}
		}
		return m_FilteredEntities;
	}

	// ============================================================
	// 绘制单个实体节点
	// ============================================================
	void SceneHierarchyPanel::DrawEntityNode(const CachedEntity& cached)
	{
		ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_FramePadding |
			ImGuiTreeNodeFlags_Leaf;  // 没有子节点

		Entity entity{ cached.Handle, m_Context.get() };
		if (m_SelectedEntity == entity)
		{
			flag |= ImGuiTreeNodeFlags_Selected;
		}

		bool opened = ImGui::TreeNodeEx(
			(void*)(uint64_t)cached.Handle,
			flag,
			"%s", cached.Tag.c_str());

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				m_Context->DestroyEntity(entity);
				m_CacheDirty = true;
			}
			ImGui::EndPopup();
		}

		if (ImGui::IsItemClicked())
		{
			m_SelectedEntity = entity;
		}

		if (opened)
		{
			ImGui::TreePop();
		}
	}

	// ============================================================
	// 属性面板
	// ============================================================
	void SceneHierarchyPanel::DrawComponents(const Entity& entity)
	{
		ImGui::Begin("Properties");
		{
			if (entity)
			{
				// Tag
				if (auto* tag = entity.GetComponent<TagComponent>())
				{
					char buffer[128];
					memset(buffer, 0, sizeof(buffer));
					strcpy_s(buffer, sizeof(buffer), tag->Tag.c_str());

					if (ImGui::InputText("##Tag", buffer, sizeof(buffer),
						ImGuiInputTextFlags_EnterReturnsTrue))
					{
						tag->Tag = buffer;
						m_CacheDirty = true;
					}
				}

				// Transform
				if (auto* transform = entity.GetComponent<TransformComponent>())
				{
					if (ImGui::CollapsingHeader("Transform",
						ImGuiTreeNodeFlags_DefaultOpen))
					{
						DrawVec3Control("Location", transform->Location);
						DrawVec3Control("Rotation", transform->Rotation);
						DrawVec3Control("Scale", transform->Scale, 1.f);
					}
				}

				// Sprite
				if (auto* sprite = entity.GetComponent<SpriteComponent>())
				{
					if (ImGui::CollapsingHeader("Sprite",
						ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::ColorEdit4("Color", glm::value_ptr(sprite->Color));
					}
				}
			}
		}
		ImGui::End();
	}
}