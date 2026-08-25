#include <Editor/UI/Style/EditorStyle.h>
#include <string>
#include <filesystem>
#include <Core/Config/Paths.h>
#include <Core/AssetManagent/AssetMgr.h>
#include <Core/Log.h>

namespace DM
{
	// ================ 运行时设置 ================
	EditorStyleSettings& EditorStyle::GetSettings()
	{
		static EditorStyleSettings s;
		return s;
	}

	// ================ 颜色应用 ================
	// 将调色板映射到 ImGui 全部颜色槽位
	void EditorStyle::ApplyPalette(const EditorStyleSettings& p)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;

		// ---------------- 文本 ----------------
		colors[ImGuiCol_Text] = p.Text;
		colors[ImGuiCol_TextDisabled] = p.TextDisabled;
		colors[ImGuiCol_TextSelectedBg] = ImVec4(p.Accent.x, p.Accent.y, p.Accent.z, 0.35f);

		// ---------------- 窗口/容器 ----------------
		colors[ImGuiCol_WindowBg] = p.Bg0;
		colors[ImGuiCol_ChildBg] = p.Bg0;
		colors[ImGuiCol_PopupBg] = p.Bg1;
		colors[ImGuiCol_MenuBarBg] = p.Bg1;
		colors[ImGuiCol_TitleBg] = p.Bg0;
		colors[ImGuiCol_TitleBgActive] = p.Bg1;
		colors[ImGuiCol_TitleBgCollapsed] = p.Bg0;

		// ---------------- 边框/分隔线 ----------------
		colors[ImGuiCol_Border] = p.Border;
		colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		colors[ImGuiCol_Separator] = p.Border;
		colors[ImGuiCol_SeparatorHovered] = p.Hovered;
		colors[ImGuiCol_SeparatorActive] = p.Active;

		// ---------------- 基础控件 ----------------
		colors[ImGuiCol_FrameBg] = p.Bg2;
		colors[ImGuiCol_FrameBgHovered] = p.Hovered;
		colors[ImGuiCol_FrameBgActive] = p.Active;

		// ---------------- 按钮/表头 ----------------
		colors[ImGuiCol_Button] = p.Bg2;
		colors[ImGuiCol_ButtonHovered] = p.Hovered;
		colors[ImGuiCol_ButtonActive] = p.Active;
		colors[ImGuiCol_Header] = p.Bg2;
		colors[ImGuiCol_HeaderHovered] = p.Hovered;
		colors[ImGuiCol_HeaderActive] = p.Active;

		// ---------------- 滚动条 ----------------
		colors[ImGuiCol_ScrollbarBg] = p.Bg2;
		colors[ImGuiCol_ScrollbarGrab] = p.Hovered;
		colors[ImGuiCol_ScrollbarGrabHovered] = p.Active;
		colors[ImGuiCol_ScrollbarGrabActive] = p.Active;

		// ---------------- 交互标记/手柄 ----------------
		colors[ImGuiCol_CheckMark] = p.Accent;
		colors[ImGuiCol_SliderGrab] = p.Accent;
		colors[ImGuiCol_SliderGrabActive] = p.Active;
		colors[ImGuiCol_ResizeGrip] = p.Bg2;
		colors[ImGuiCol_ResizeGripHovered] = p.Hovered;
		colors[ImGuiCol_ResizeGripActive] = p.Active;

		// ---------------- 标签页(1.93 新槽位) ----------------
		colors[ImGuiCol_Tab] = p.Bg2;
		colors[ImGuiCol_TabHovered] = p.Hovered;
		colors[ImGuiCol_TabSelected] = p.Active;
		colors[ImGuiCol_TabSelectedOverline] = p.Accent;          // 选中 tab 顶部高亮条
		colors[ImGuiCol_TabDimmed] = p.Bg1;
		colors[ImGuiCol_TabDimmedSelected] = p.Bg2;
		colors[ImGuiCol_TabDimmedSelectedOverline] = p.Accent;    // 失焦时选中 tab 高亮条

		// ---------------- 图表 ----------------
		colors[ImGuiCol_PlotLines] = p.Accent;
		colors[ImGuiCol_PlotLinesHovered] = p.Active;
		colors[ImGuiCol_PlotHistogram] = p.Accent;
		colors[ImGuiCol_PlotHistogramHovered] = p.Active;

		// ---------------- 表格 ----------------
		colors[ImGuiCol_TableHeaderBg] = p.Bg1;
		colors[ImGuiCol_TableBorderStrong] = p.Border;
		colors[ImGuiCol_TableBorderLight] = ImVec4(p.Border.x, p.Border.y, p.Border.z, 0.5f);
		colors[ImGuiCol_TableRowBg] = p.Bg0;
		colors[ImGuiCol_TableRowBgAlt] = p.Bg1;

		// ---------------- 拖放/导航 ----------------
		colors[ImGuiCol_DragDropTarget] = ImVec4(p.Accent.x, p.Accent.y, p.Accent.z, 0.8f);
		colors[ImGuiCol_DragDropTargetBg] = ImVec4(p.Accent.x, p.Accent.y, p.Accent.z, 0.15f);
		colors[ImGuiCol_NavHighlight] = p.Active;
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.35f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.45f);
	}

	// ================ 形状布局应用 ================
	void EditorStyle::ApplyLayout(const EditorStyleSettings& s)
	{
		ImGuiStyle& style = ImGui::GetStyle();

		// ---------------- 间距/内边距 ----------------
		style.WindowPadding = s.WindowPadding;
		style.FramePadding = s.FramePadding;
		style.CellPadding = s.CellPadding;
		style.ItemSpacing = s.ItemSpacing;
		style.ItemInnerSpacing = s.ItemInnerSpacing;
		style.IndentSpacing = s.IndentSpacing;
		style.ScrollbarSize = s.ScrollbarSize;
		style.ScrollbarPadding = s.ScrollbarPadding;
		style.GrabMinSize = s.GrabMinSize;

		// ---------------- 边框 ----------------
		style.WindowBorderSize = s.WindowBorderSize;
		style.ChildBorderSize = s.ChildBorderSize;
		style.PopupBorderSize = s.PopupBorderSize;
		style.FrameBorderSize = s.FrameBorderSize;
		style.TabBorderSize = s.TabBorderSize;
		style.TabBarBorderSize = s.TabBarBorderSize;
		style.TabBarOverlineSize = s.TabBarOverlineSize;
		style.DockingSeparatorSize = s.DockingSeparatorSize;
		style.WindowBorderHoverPadding = s.WindowBorderHoverPadding;
		style.SeparatorSize = s.SeparatorSize;

		// ---------------- 圆角 ----------------
		style.WindowRounding = s.WindowRounding;
		style.ChildRounding = s.ChildRounding;
		style.FrameRounding = s.FrameRounding;
		style.PopupRounding = s.PopupRounding;
		style.ScrollbarRounding = s.ScrollbarRounding;
		style.GrabRounding = s.GrabRounding;
		style.TabRounding = s.TabRounding;
		style.MenuItemRounding = s.MenuItemRounding;
		style.ImageRounding = s.ImageRounding;
		style.ImageBorderSize = s.ImageBorderSize;
		style.TreeLinesSize = s.TreeLinesSize;
		style.TreeLinesRounding = s.TreeLinesRounding;
		style.DragDropTargetRounding = s.DragDropTargetRounding;
		style.DragDropTargetBorderSize = s.DragDropTargetBorderSize;
		style.DragDropTargetPadding = s.DragDropTargetPadding;

		// ---------------- 文本对齐/装饰 ----------------
		style.WindowTitleAlign = s.WindowTitleAlign;
		style.ButtonTextAlign = s.ButtonTextAlign;
		style.SelectableTextAlign = s.SelectableTextAlign;
		style.WindowMenuButtonPosition = static_cast<ImGuiDir>(s.WindowMenuButtonPosition);
		style.MouseCursorScale = s.MouseCursorScale;

		// ---------------- 显示/抗锯齿 ----------------
		style.DisplayWindowPadding = s.DisplayWindowPadding;
		style.DisplaySafeAreaPadding = s.DisplaySafeAreaPadding;
		style.AntiAliasedLines = s.AntiAliasedLines;
		style.AntiAliasedFill = s.AntiAliasedFill;
		style.CurveTessellationMaxError = s.CurveTessellationMaxError;
		style.CircleTessellationMaxError = s.CircleTessellationMaxError;
	}


	namespace
	{
		EditorStyle::ETheme& CurrentTheme()
		{
			static EditorStyle::ETheme current = EditorStyle::ETheme::DarkLuxury;
			return current;
		}
	}

	void EditorStyle::ApplyCurrentStyle()
	{
		const EditorStyleSettings& s = GetSettings();
		// 先形状后配色：配色只写颜色槽，圆角/间距由布局统一管理
		ApplyLayout(s);
		ApplyPalette(s);
	}

	void EditorStyle::ResetToDefaults()
	{
		GetSettings() = EditorStyleSettings{};
		ApplyCurrentStyle();
	}

	void EditorStyle::ApplyTheme(ETheme theme)
	{
		CurrentTheme() = theme;
		ApplyCurrentStyle();
	}

	void EditorStyle::SaveConfig(const std::string& path)
	{
		mirror::Serialize(path, &GetSettings());
		LOG_CORE_INFO("Editor style saved to {}", path);
	}

	void EditorStyle::LoadConfig(const std::string& path)
	{
		// 先重置为默认，再覆盖配置文件中存在的字段(缺失字段/解析失败均保持默认)
		 auto& s = GetSettings();
		 s = EditorStyleSettings{};
		 if (!std::filesystem::exists(path))return;

		 mirror::Deserialize(path, &s);

	}

	const char* EditorStyle::GetConfigPath()
	{
		// 外观配置统一存放在 <ProjectRoot>/Config/Style.json
		static const std::string path = Paths::ConfigFile("Style.json").string();
		return path.c_str();
	}

	void EditorStyle::SetDefaultStyle()
	{
		ImGuiIO& io = ImGui::GetIO();
		const std::string Neuton_Bold = (Paths::AssetRoot() / "Font" / "Neuton" / "Neuton-Bold.ttf").string();
		const std::string Neuton_Regular = (Paths::AssetRoot() / "Font" / "Neuton" / "Neuton-Regular.ttf").string();
		io.Fonts->AddFontFromFileTTF(Neuton_Bold.c_str(), 20.f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF(Neuton_Regular.c_str(), 20.f);

		// 中文字形：Neuton 为纯拉丁字体，不包含中文字形，必须再合并一个中文字体，
		// 否则界面中的中文(面板标题/标签等)会渲染成乱码。
		// 字体来源优先项目资产(可移植)，缺失时回退到系统字体(微软雅黑/黑体)。
		const std::string zhFontCandidates[] = {
			(Paths::AssetRoot() / "Font" / "msyh.ttc").string(),
			"C:/Windows/Fonts/msyh.ttc",
			"C:/Windows/Fonts/msyhbd.ttc",
			"C:/Windows/Fonts/simhei.ttf",
		};
		std::string zhFont;
		for (const auto& candidate : zhFontCandidates)
		{
			if (std::filesystem::exists(candidate)) { zhFont = candidate; break; }
		}
		if (!zhFont.empty())
		{
			ImFontConfig cfg;
			cfg.MergeMode = true;          // 合并进上一个字体(Regular/FontDefault)，不新增字体槽
			cfg.GlyphMinAdvanceX = 20.0f;  // 与主字号一致，避免中文偏窄
			io.Fonts->AddFontFromFileTTF(zhFont.c_str(), 20.0f, &cfg, io.Fonts->GetGlyphRangesChineseFull());
		}

		LoadConfig(GetConfigPath());
		ApplyTheme(ETheme::DarkLuxury);
	}

	EditorStyle::ETheme EditorStyle::GetCurrentTheme()
	{
		return CurrentTheme();
	}

	const char* EditorStyle::GetThemeName(ETheme /*theme*/)
	{
		return "Dark Luxury";
	}
}
