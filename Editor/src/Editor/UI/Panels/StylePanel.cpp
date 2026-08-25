#include "Editor/UI/Panels/StylePanel.h"
#include <Editor/UI/EditorUI.h>

#include <Editor/UI/Style/EditorStyle.h>
namespace DM
{
	namespace
	{
		// 拖动滑块实时应用外观
		inline void ApplyStyleOnChange()
		{
			EditorStyle::ApplyCurrentStyle();
		}

		// 颜色编辑(带实时应用)
		bool ColorEdit(const char* label, ImVec4& color)
		{
			const bool changed = EditorUI::ColorEdit4(label, &color.x, ImGuiColorEditFlags_AlphaBar);
			if (changed) ApplyStyleOnChange();
			return changed;
		}

		// 浮点滑块(带范围与实时应用)
		bool SliderFloat(const char* label, float& value, float min, float max, const char* fmt = "%.2f")
		{
			const bool changed = EditorUI::SliderFloat(label, &value, min, max, fmt);
			if (changed) ApplyStyleOnChange();
			return changed;
		}

		// 二维向量滑块(带范围与实时应用)
		bool SliderFloat2(const char* label, ImVec2& value, float min, float max, const char* fmt = "%.2f")
		{
			const bool changed = EditorUI::SliderFloat2(label, &value.x, min, max, fmt);
			if (changed) ApplyStyleOnChange();
			return changed;
		}

		// 整型滑块(带范围与实时应用)
		bool SliderInt(const char* label, int& value, int min, int max)
		{
			const bool changed = EditorUI::SliderInt(label, &value, min, max);
			if (changed) ApplyStyleOnChange();
			return changed;
		}

		void RenderColors(EditorStyleSettings& s)
		{
			if (EditorUI::CollapsingHeader("Colors"))
			{
				ColorEdit("Background 0 (窗口/子窗口)", s.Bg0);
				ColorEdit("Background 1 (菜单栏/弹窗)", s.Bg1);
				ColorEdit("Background 2 (控件背景)", s.Bg2);
				ColorEdit("Hovered (悬浮)", s.Hovered);
				ColorEdit("Active (按下/选中)", s.Active);
				ColorEdit("Border (边框/分隔线)", s.Border);
				ColorEdit("Text (文本)", s.Text);
				ColorEdit("Text Disabled (禁用文本)", s.TextDisabled);
				ColorEdit("Accent (强调色)", s.Accent);
			}
		}

		void RenderSpacing(EditorStyleSettings& s)
		{
			if (EditorUI::CollapsingHeader("Spacing (间距/内边距)"))
			{
				SliderFloat2("Window Padding", s.WindowPadding, 0.0f, 60.0f);
				SliderFloat2("Frame Padding", s.FramePadding, 0.0f, 30.0f);
				SliderFloat2("Cell Padding", s.CellPadding, 0.0f, 30.0f);
				SliderFloat2("Item Spacing", s.ItemSpacing, 0.0f, 40.0f);
				SliderFloat2("Item Inner Spacing", s.ItemInnerSpacing, 0.0f, 40.0f);
				SliderFloat("Indent Spacing", s.IndentSpacing, 0.0f, 60.0f);
				SliderFloat("Scrollbar Size", s.ScrollbarSize, 4.0f, 30.0f);
				SliderFloat("Scrollbar Padding", s.ScrollbarPadding, 0.0f, 20.0f);
				SliderFloat("Grab Min Size", s.GrabMinSize, 2.0f, 40.0f);
			}
		}

		void RenderBorders(EditorStyleSettings& s)
		{
			if (EditorUI::CollapsingHeader("Borders (边框)"))
			{
				SliderFloat("Window Border", s.WindowBorderSize, 0.0f, 8.0f);
				SliderFloat("Child Border", s.ChildBorderSize, 0.0f, 8.0f);
				SliderFloat("Popup Border", s.PopupBorderSize, 0.0f, 8.0f);
				SliderFloat("Frame Border", s.FrameBorderSize, 0.0f, 8.0f);
				SliderFloat("Tab Border", s.TabBorderSize, 0.0f, 8.0f);
				SliderFloat("Tab Bar Border", s.TabBarBorderSize, 0.0f, 8.0f);
				SliderFloat("Tab Bar Overline", s.TabBarOverlineSize, 0.0f, 12.0f);
				SliderFloat("Docking Separator", s.DockingSeparatorSize, 0.0f, 12.0f);
				SliderFloat("Window Border Hover Padding", s.WindowBorderHoverPadding, 0.0f, 20.0f);
				SliderFloat("Separator", s.SeparatorSize, 0.0f, 8.0f);
			}
		}

		void RenderRounding(EditorStyleSettings& s)
		{
			if (EditorUI::CollapsingHeader("Rounding (圆角)"))
			{
				SliderFloat("Window", s.WindowRounding, 0.0f, 40.0f);
				SliderFloat("Child", s.ChildRounding, 0.0f, 40.0f);
				SliderFloat("Frame", s.FrameRounding, 0.0f, 40.0f);
				SliderFloat("Popup", s.PopupRounding, 0.0f, 40.0f);
				SliderFloat("Scrollbar", s.ScrollbarRounding, 0.0f, 40.0f);
				SliderFloat("Grab", s.GrabRounding, 0.0f, 40.0f);
				SliderFloat("Tab", s.TabRounding, 0.0f, 40.0f);
				SliderFloat("Menu Item", s.MenuItemRounding, 0.0f, 40.0f);
				SliderFloat("Image", s.ImageRounding, 0.0f, 40.0f);
				SliderFloat("Image Border", s.ImageBorderSize, 0.0f, 8.0f);
				SliderFloat("Tree Lines", s.TreeLinesSize, 0.0f, 10.0f);
				SliderFloat("Tree Lines Rounding", s.TreeLinesRounding, 0.0f, 30.0f);
				SliderFloat("DragDrop Target Rounding", s.DragDropTargetRounding, 0.0f, 40.0f);
				SliderFloat("DragDrop Target Border", s.DragDropTargetBorderSize, 0.0f, 10.0f);
				SliderFloat("DragDrop Target Padding", s.DragDropTargetPadding, 0.0f, 30.0f);
			}
		}

		void RenderTextAlign(EditorStyleSettings& s)
		{
			if (EditorUI::CollapsingHeader("Text / Misc"))
			{
				SliderFloat2("Window Title Align", s.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
				SliderFloat2("Button Text Align", s.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
				SliderFloat2("Selectable Text Align", s.SelectableTextAlign, 0.0f, 1.0f, "%.2f");
				SliderInt("Window Menu Button", s.WindowMenuButtonPosition, 0, 1);
				SliderFloat("Mouse Cursor Scale", s.MouseCursorScale, 0.5f, 3.0f);
				SliderFloat2("Display Window Padding", s.DisplayWindowPadding, 0.0f, 60.0f);
				SliderFloat2("Display Safe Area Padding", s.DisplaySafeAreaPadding, 0.0f, 30.0f);

				if (EditorUI::Checkbox("Anti-aliased Lines", &s.AntiAliasedLines)) ApplyStyleOnChange();
				if (EditorUI::Checkbox("Anti-aliased Fill", &s.AntiAliasedFill)) ApplyStyleOnChange();
				SliderFloat("Curve Tessellation Error", s.CurveTessellationMaxError, 0.1f, 2.0f, "%.2f");
				SliderFloat("Circle Tessellation Error", s.CircleTessellationMaxError, 0.1f, 2.0f, "%.2f");
			}
		}
	}

	void StylePanel::Render()
	{
		if (!m_Open) return;

		if (EditorUI::Begin("Style Settings", &m_Open))
		{
			EditorStyleSettings& s = EditorStyle::GetSettings();

			RenderColors(s);
			RenderSpacing(s);
			RenderBorders(s);
			RenderRounding(s);
			RenderTextAlign(s);

			EditorUI::Separator();

			if (EditorUI::Button("Save Appearance"))
			{
				EditorStyle::SaveConfig(EditorStyle::GetConfigPath());
			}
			EditorUI::SameLine();
			if (EditorUI::Button("Reset Defaults"))
			{
				EditorStyle::ResetToDefaults();
			}
			EditorUI::TextDisabled("Config: %s", EditorStyle::GetConfigPath());
		}
		EditorUI::End();
	}
}
