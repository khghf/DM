#pragma once
// ============================================================
// EditorUI —— 编辑器 UI 薄封装
//
// 职责：
//   - 作为面板/菜单等 UI 代码与 ImGui 之间的唯一依赖点
//   - 面板代码应 #include <Editor/UI/EditorUI.h> 并通过 EditorUI:: 命名空间调用
//   - 仅 UI/Backend 层(ImGuiInitializer / ImGuiRenderer / EditorShortcuts 等)允许直接包含 imgui.h
//
// 约定：
//   - ImVec2 / ImVec4 / ImGuiIO / ImGuiKey 等类型沿用 imgui 定义(物理隔离调用点，类型复用)
//   - 未来若替换底层 UI 库，只需在本文件内更换实现，面板代码无需改动
// ============================================================
#include <imgui.h>
#include <cstdarg>

namespace DM
{
	namespace EditorUI
	{
		// ---- 窗口 ----
		inline bool Begin(const char* name, bool* pOpen = nullptr, ImGuiWindowFlags flags = 0) { return ImGui::Begin(name, pOpen, flags); }
		inline void End() { ImGui::End(); }
		inline bool BeginPanel(const char* name, bool* pOpen = nullptr, ImGuiWindowFlags flags = 0) { return Begin(name, pOpen, flags); }
		inline void EndPanel() { End(); }
		inline bool BeginChild(const char* strId, const ImVec2& size = ImVec2(0, 0), bool border = false, ImGuiWindowFlags flags = 0) { return ImGui::BeginChild(strId, size, border, flags); }
		inline void EndChild() { ImGui::EndChild(); }
		inline bool BeginMenuBar() { return ImGui::BeginMenuBar(); }
		inline void EndMenuBar() { ImGui::EndMenuBar(); }
		inline bool BeginMenu(const char* label, bool enabled = true) { return ImGui::BeginMenu(label, enabled); }
		inline void EndMenu() { ImGui::EndMenu(); }
		inline void SetNextWindowPos(const ImVec2& pos, ImGuiCond cond = 0, const ImVec2& pivot = ImVec2(0, 0)) { ImGui::SetNextWindowPos(pos, cond, pivot); }
		inline void SetNextWindowSize(const ImVec2& size, ImGuiCond cond = 0) { ImGui::SetNextWindowSize(size, cond); }
		inline void Image(ImTextureRef userTextureId, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1)) { ImGui::Image(userTextureId, size, uv0, uv1); }
		inline void ImageWithBg(ImTextureRef userTextureId, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& bgCol = ImVec4(0, 0, 0, 0), const ImVec4& tintCol = ImVec4(1, 1, 1, 1)) { ImGui::ImageWithBg(userTextureId, size, uv0, uv1, bgCol, tintCol); }

		// ---- 弹窗 / 弹出上下文 / 拖拽 ----
		inline void OpenPopup(const char* strId, ImGuiPopupFlags flags = 0) { ImGui::OpenPopup(strId, flags); }
		inline bool BeginPopupModal(const char* name, bool* pOpen = nullptr, ImGuiWindowFlags flags = 0) { return ImGui::BeginPopupModal(name, pOpen, flags); }
		inline void EndPopup() { ImGui::EndPopup(); }
		inline void CloseCurrentPopup() { ImGui::CloseCurrentPopup(); }
		inline bool BeginPopupContextItem(const char* strId = nullptr, ImGuiPopupFlags popupFlags = 1) { return ImGui::BeginPopupContextItem(strId, popupFlags); }
		inline bool BeginPopupContextWindow(const char* strId = nullptr, ImGuiPopupFlags popupFlags = 1) { return ImGui::BeginPopupContextWindow(strId, popupFlags); }
		inline bool BeginDragDropSource(ImGuiDragDropFlags flags = 0) { return ImGui::BeginDragDropSource(flags); }
		inline void EndDragDropSource() { ImGui::EndDragDropSource(); }
		inline bool SetDragDropPayload(const char* type, const void* data, size_t size, ImGuiCond cond = 0) { return ImGui::SetDragDropPayload(type, data, size, cond); }
		inline bool BeginDragDropTarget() { return ImGui::BeginDragDropTarget(); }
		inline void EndDragDropTarget() { ImGui::EndDragDropTarget(); }
		inline const ImGuiPayload* AcceptDragDropPayload(const char* type, ImGuiDragDropFlags flags = 0) { return ImGui::AcceptDragDropPayload(type, flags); }

		// ---- 菜单项 ----
		inline bool MenuItem(const char* label, const char* shortcut, bool selected = false, bool enabled = true) { return ImGui::MenuItem(label, shortcut, selected, enabled); }
		inline bool MenuItem(const char* label, const char* shortcut, bool* pSelected, bool enabled = true) { return ImGui::MenuItem(label, shortcut, pSelected, enabled); }
		inline bool MenuItem(const char* label) { return ImGui::MenuItem(label); }

		// ---- 基础控件 ----
		inline bool Button(const char* label, const ImVec2& size = ImVec2(0, 0)) { return ImGui::Button(label, size); }
		inline bool Checkbox(const char* label, bool* v) { return ImGui::Checkbox(label, v); }
		inline void SameLine() { ImGui::SameLine(); }
		inline void Separator() { ImGui::Separator(); }
		inline void Spacing() { ImGui::Spacing(); }

		// ---- 文本(变参经 V 版本转发) ----
		inline void Text(const char* fmt, ...) { va_list args; va_start(args, fmt); ImGui::TextV(fmt, args); va_end(args); }
		inline void TextUnformatted(const char* text) { ImGui::TextUnformatted(text); }
		inline void TextDisabled(const char* fmt, ...) { va_list args; va_start(args, fmt); ImGui::TextDisabledV(fmt, args); va_end(args); }
		inline void TextColored(const ImVec4& col, const char* fmt, ...) { va_list args; va_start(args, fmt); ImGui::TextColoredV(col, fmt, args); va_end(args); }
		inline void TextWrapped(const char* fmt, ...) { va_list args; va_start(args, fmt); ImGui::TextWrappedV(fmt, args); va_end(args); }
		inline void BulletText(const char* fmt, ...) { va_list args; va_start(args, fmt); ImGui::BulletTextV(fmt, args); va_end(args); }
		inline void SetTooltip(const char* fmt, ...) { va_list args; va_start(args, fmt); ImGui::SetTooltipV(fmt, args); va_end(args); }
		inline void SeparatorText(const char* label) { ImGui::SeparatorText(label); }

		// ---- 输入控件 ----
		inline bool InputText(const char* label, char* buf, size_t bufSize, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* userData = nullptr) { return ImGui::InputText(label, buf, bufSize, flags, callback, userData); }
		inline bool DragFloat(const char* label, float* v, float vSpeed = 1.0f, float vMin = 0.0f, float vMax = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0) { return ImGui::DragFloat(label, v, vSpeed, vMin, vMax, format, flags); }
		inline bool DragInt(const char* label, int* v, float vSpeed = 1.0f, int vMin = 0, int vMax = 0, const char* format = "%d", ImGuiSliderFlags flags = 0) { return ImGui::DragInt(label, v, vSpeed, vMin, vMax, format, flags); }
		inline bool SliderFloat(const char* label, float* v, float vMin, float vMax, const char* format = "%.3f", ImGuiSliderFlags flags = 0) { return ImGui::SliderFloat(label, v, vMin, vMax, format, flags); }
		inline bool SliderFloat2(const char* label, float* v, float vMin, float vMax, const char* format = "%.3f", ImGuiSliderFlags flags = 0) { return ImGui::SliderFloat2(label, v, vMin, vMax, format, flags); }
		inline bool SliderInt(const char* label, int* v, int vMin, int vMax, const char* format = "%d", ImGuiSliderFlags flags = 0) { return ImGui::SliderInt(label, v, vMin, vMax, format, flags); }
		inline bool ColorEdit4(const char* label, float col[4], ImGuiColorEditFlags flags = 0) { return ImGui::ColorEdit4(label, col, flags); }
		inline bool ImageButton(const char* strId, ImTextureRef userTextureId, const ImVec2& size = ImVec2(0, 0), const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& bgCol = ImVec4(0, 0, 0, 0), const ImVec4& tintCol = ImVec4(1, 1, 1, 1)) { return ImGui::ImageButton(strId, userTextureId, size, uv0, uv1, bgCol, tintCol); }
		inline bool CollapsingHeader(const char* label, ImGuiTreeNodeFlags flags = 0) { return ImGui::CollapsingHeader(label, flags); }
		inline bool TreeNodeEx(const void* ptrId, ImGuiTreeNodeFlags flags, const char* fmt, ...) { va_list args; va_start(args, fmt); const bool b = ImGui::TreeNodeExV(ptrId, flags, fmt, args); va_end(args); return b; }
		inline void TreePop() { ImGui::TreePop(); }

		// ---- ID / 布局 ----
		inline void PushID(const char* strId) { ImGui::PushID(strId); }
		inline void PushID(int intId) { ImGui::PushID(intId); }
		inline void PopID() { ImGui::PopID(); }
		inline void Columns(int count = 1, const char* id = nullptr, bool border = true) { ImGui::Columns(count, id, border); }
		inline void NextColumn() { ImGui::NextColumn(); }
		inline void SetColumnWidth(int columnIndex, float width) { ImGui::SetColumnWidth(columnIndex, width); }
		// PushMultiItemsWidths 已移入 imgui_internal，此处用公共 API 等效实现(行为与 imgui_internal 版一致)：
		// 每个组件均分总宽，共压入 components 个宽度，后续必须对每个组件各调用一次 PopItemWidth()
		inline void PushMultiItemsWidths(int components, float widthFull)
		{
			const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
			const float wItems = widthFull - spacing * (components - 1);
			float prevSplit = wItems;
			for (int i = components - 1; i > 0; i--)
			{
				const float nextSplit = (float)(int)(wItems * i / components); // 对齐 IM_TRUNC 行为
				ImGui::PushItemWidth(prevSplit - nextSplit > 1.0f ? prevSplit - nextSplit : 1.0f);
				prevSplit = nextSplit;
			}
			ImGui::PushItemWidth(prevSplit > 1.0f ? prevSplit : 1.0f);
		}
		inline void PopItemWidth() { ImGui::PopItemWidth(); }
		inline float CalcItemWidth() { return ImGui::CalcItemWidth(); }

		// ---- 样式 ----
		inline void PushStyleColor(ImGuiCol idx, const ImVec4& col) { ImGui::PushStyleColor(idx, col); }
		inline void PopStyleColor(int count = 1) { ImGui::PopStyleColor(count); }
		inline void PushStyleVar(ImGuiStyleVar idx, float val) { ImGui::PushStyleVar(idx, val); }
		inline void PushStyleVar(ImGuiStyleVar idx, const ImVec2& val) { ImGui::PushStyleVar(idx, val); }
		inline void PopStyleVar(int count = 1) { ImGui::PopStyleVar(count); }

		// ---- 查询 / 状态 ----
		inline bool IsItemHovered(ImGuiHoveredFlags flags = 0) { return ImGui::IsItemHovered(flags); }
		inline bool IsItemClicked(ImGuiMouseButton mouseButton = 0) { return ImGui::IsItemClicked(mouseButton); }
		inline bool IsAnyItemHovered() { return ImGui::IsAnyItemHovered(); }
		inline bool IsWindowHovered(ImGuiHoveredFlags flags = 0) { return ImGui::IsWindowHovered(flags); }
		inline bool IsWindowFocused(ImGuiFocusedFlags flags = 0) { return ImGui::IsWindowFocused(flags); }
		inline bool IsMouseClicked(ImGuiMouseButton button, bool repeat = false) { return ImGui::IsMouseClicked(button, repeat); }
		inline bool IsMouseDoubleClicked(ImGuiMouseButton button) { return ImGui::IsMouseDoubleClicked(button); }
		inline bool IsKeyReleased(ImGuiKey key) { return ImGui::IsKeyReleased(key); }
		inline bool IsKeyPressed(ImGuiKey key, bool repeat = true) { return ImGui::IsKeyPressed(key, repeat); }
		inline bool IsKeyChordPressed(ImGuiKeyChord chord) { return ImGui::IsKeyChordPressed(chord); }
		inline void SetClipboardText(const char* text) { ImGui::SetClipboardText(text); }
		inline ImGuiIO& GetIO() { return ImGui::GetIO(); }
		inline ImGuiStyle& GetStyle() { return ImGui::GetStyle(); }
		inline ImVec2 GetContentRegionAvail() { return ImGui::GetContentRegionAvail(); }
		inline ImVec2 GetItemRectMin() { return ImGui::GetItemRectMin(); }
		inline ImVec2 GetItemRectMax() { return ImGui::GetItemRectMax(); }
		inline ImVec2 GetCursorScreenPos() { return ImGui::GetCursorScreenPos(); }
		inline ImVec2 GetMousePos() { return ImGui::GetMousePos(); }
		inline ImVec2 GetWindowPos() { return ImGui::GetWindowPos(); }
		inline ImDrawList* GetWindowDrawList() { return ImGui::GetWindowDrawList(); }
		inline float GetTextLineHeightWithSpacing() { return ImGui::GetTextLineHeightWithSpacing(); }
		inline float GetFontSize() { return ImGui::GetFontSize(); }
	}
}
