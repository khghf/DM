#include <DMPCH.h>
#include "Editor/UI/Panels/PropertiesPanel.h"
#include <Editor/UI/EditorUI.h>
#include <Framework/Base/Entity.h>
#include <Framework/Component/Component.h>
#include <Framework/Component/TransformComponent.h>
#include <Framework/Component/SpriteComponent.h>
#include <Foundation/Math/Vector.h>
#include <Editor/Core/EditorContext.h>
#include <Editor/Services/SelectionManager.h>
#include <glm/gtc/type_ptr.hpp>

namespace DM
{
	// ============================================================
	// Vec3 控件
	// ============================================================
	static bool DrawVec3Control(const std::string& label, Vector3& val,
		float resetVal = 0.f, float columnWidth = 100.f)
	{
		bool changed = false;
		EditorUI::PushID(label.c_str());
		EditorUI::Columns(2);
		EditorUI::SetColumnWidth(0, columnWidth);
		EditorUI::Text(label.c_str());
		EditorUI::NextColumn();
		EditorUI::PushMultiItemsWidths(3, EditorUI::CalcItemWidth());

		float lineHeight = EditorUI::GetFontSize() + EditorUI::GetStyle().FramePadding.y * 2.f;
		ImVec2 buttonSize = { lineHeight + 3.f, lineHeight };
		float baseColor = 0.8f;

		ImGuiIO& io = EditorUI::GetIO();
		ImFont* defaultFont = io.FontDefault;
		io.FontDefault = io.Fonts->Fonts[1];
		float dragSpeed = 0.001f;

		// X
		EditorUI::PushStyleColor(ImGuiCol_Button, { baseColor, 0.f, 0.f, 1.f });
		EditorUI::PushStyleColor(ImGuiCol_ButtonHovered, { 1.f, 0.f, 0.f, 1.f });
		EditorUI::PushStyleColor(ImGuiCol_ButtonActive, { baseColor, 0.f, 0.f, 1.f });
		if (EditorUI::Button("X", buttonSize)) { val.x = resetVal; changed = true; }
		EditorUI::PopStyleColor(3);
		EditorUI::SameLine();
		changed |= EditorUI::DragFloat("##X", &val.x, dragSpeed);
		EditorUI::PopItemWidth();
		EditorUI::SameLine();

		// Y
		EditorUI::PushStyleColor(ImGuiCol_Button, { 0.f, baseColor, 0.f, 1.f });
		EditorUI::PushStyleColor(ImGuiCol_ButtonHovered, { 0.f, 1.f, 0.f, 1.f });
		EditorUI::PushStyleColor(ImGuiCol_ButtonActive, { 0.f, baseColor, 0.f, 1.f });
		if (EditorUI::Button("Y", buttonSize)) { val.y = resetVal; changed = true; }
		EditorUI::PopStyleColor(3);
		EditorUI::SameLine();
		changed |= EditorUI::DragFloat("##Y", &val.y, dragSpeed);
		EditorUI::PopItemWidth();
		EditorUI::SameLine();

		// Z
		EditorUI::PushStyleColor(ImGuiCol_Button, { 0.f, 0.f, baseColor, 1.f });
		EditorUI::PushStyleColor(ImGuiCol_ButtonHovered, { 0.f, 0.f, 1.f, 1.f });
		EditorUI::PushStyleColor(ImGuiCol_ButtonActive, { 0.f, 0.f, baseColor, 1.f });
		if (EditorUI::Button("Z", buttonSize)) { val.z = resetVal; changed = true; }
		EditorUI::PopStyleColor(3);
		EditorUI::SameLine();
		changed |= EditorUI::DragFloat("##Z", &val.z, dragSpeed);
		EditorUI::PopItemWidth();

		io.FontDefault = defaultFont;
		EditorUI::Columns(1);
		EditorUI::PopID();
		return changed;
	}

	// ============================================================
	// 渲染主面板
	// ============================================================
	void PropertiesPanel::Render()
	{
		EditorUI::Begin("Properties");
		{
			// 属性面板消费主选中实体(最后点击/恢复的实体)；多选时显示主选中项
			Entity entity = EditorContext::GetService<SelectionManager>()->GetPrimaryEntity();
			if (entity)
			{
				DrawComponents(entity);
			}
			else
			{
				EditorUI::TextWrapped("Select an entity in the Scene Hierarchy.");
			}
		}
		EditorUI::End();
	}

	// ============================================================
	// 组件属性编辑
	// ============================================================
	void PropertiesPanel::DrawComponents(const Entity& entity)
	{
		// Tag
		if (auto* tag = entity.GetComponent<TagComponent>())
		{
			char buffer[128];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag->Tag.c_str());

			if (EditorUI::InputText("##Tag", buffer, sizeof(buffer),
				ImGuiInputTextFlags_EnterReturnsTrue))
			{
				tag->Tag = buffer;
				EditorContext::Get()->NotifyChanged();      // 通知 Hierarchy 刷新标签
				EditorContext::Get()->NotifyWorldModified(); // 标记世界未保存修改
			}
		}

		// Transform
		if (auto* transform = entity.GetComponent<TransformComponent>())
		{
			if (EditorUI::CollapsingHeader("Transform",
				ImGuiTreeNodeFlags_DefaultOpen))
			{
				bool transformChanged = false;
				transformChanged |= DrawVec3Control("Location", transform->Location);
				transformChanged |= DrawVec3Control("Rotation", transform->Rotation);
				transformChanged |= DrawVec3Control("Scale", transform->Scale, 1.f);
				if (transformChanged) EditorContext::Get()->NotifyWorldModified();
			}
		}

		// Sprite
		if (auto* sprite = entity.GetComponent<SpriteComponent>())
		{
			if (EditorUI::CollapsingHeader("Sprite",
				ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (EditorUI::ColorEdit4("Color", glm::value_ptr(sprite->Color)))
					EditorContext::Get()->NotifyWorldModified();
			}
		}
	}
}
