#pragma once
#include <imgui.h>
#include <Core/Reflection/Mirror/include/mirror.h>
#include "Editor/UI/ImGuiSerialization.h" 
#include <string>
namespace DM
{
	struct EditorStyleSettings
	{
		// -------- 颜色 --------
		ImVec4 Bg0{ 0.1f, 0.1f, 0.1f, 1.0f };          // 主背景(窗口/子窗口)
		ImVec4 Bg1{ 0.1f, 0.1f, 0.1f, 1.0f };          // 次级背景(菜单栏/弹窗/标题激活)
		ImVec4 Bg2{ 0.2f, 0.2f, 0.2f, 1.0f };          // 控件背景(按钮/输入框/表头)
		ImVec4 Hovered{ 0.4f, 0.4f, 0.4f, 1.0f };      // 悬浮态
		ImVec4 Active{ 0.6f, 0.6f, 0.6f, 1.0f };       // 激活态(按下/选中)
		ImVec4 Border{ 0.5f, 0.4f, 0.3f, 1.0f };       // 边框/分隔线
		ImVec4 Text{ 1.0f, 1.0f, 1.0f, 1.0f };         // 普通文本
		ImVec4 TextDisabled{ 0.2f, 0.2f, 0.2f, 1.0f }; // 禁用文本
		ImVec4 Accent{ 0.9f, 0.6f, 0.2f, 1.0f };       // 强调色(香槟金)

		// -------- 间距/内边距 --------
		ImVec2 WindowPadding{ 6.5f, 15.0f };
		ImVec2 FramePadding{ 8.3f, 3.5f };
		ImVec2 CellPadding{ 4.5f, 4.7f };
		ImVec2 ItemSpacing{ 3.5f, 7.3f };
		ImVec2 ItemInnerSpacing{ 3.0f, 5.0f };
		float IndentSpacing{ 18.3f };
		float ScrollbarSize{ 12.1f };
		float ScrollbarPadding{ 3.0f };
		float GrabMinSize{ 3.5f };

		// -------- 边框 --------
		float WindowBorderSize{ 2.0f };
		float ChildBorderSize{ 1.0f };
		float PopupBorderSize{ 1.0f };
		float FrameBorderSize{ 0.0f };
		float TabBorderSize{ 1.0f };
		float TabBarBorderSize{ 1.0f };
		float TabBarOverlineSize{ 0.0f };
		float DockingSeparatorSize{ 2.0f };
		float WindowBorderHoverPadding{ 4.0f };
		float SeparatorSize{ 1.0f };

		// -------- 圆角 --------
		float WindowRounding{ 5.0f };
		float ChildRounding{ 5.1f };
		float FrameRounding{ 3.1f };
		float PopupRounding{ 5.1f };
		float ScrollbarRounding{ 15.8f };
		float GrabRounding{ 0.0f };
		float TabRounding{ 11.6f };
		float MenuItemRounding{ 10.5f };
		float ImageRounding{ 4.6f };
		float ImageBorderSize{ 0.1f };
		float TreeLinesSize{ 1.6f };
		float TreeLinesRounding{ 6.0f };
		float DragDropTargetRounding{ 8.0f };
		float DragDropTargetBorderSize{ 2.0f };
		float DragDropTargetPadding{ 5.8f };

		// -------- 文本对齐/装饰 --------
		ImVec2 WindowTitleAlign{ 0.5f, 0.5f };
		ImVec2 ButtonTextAlign{ 0.5f, 0.5f };
		ImVec2 SelectableTextAlign{ 0.0f, 0.0f };
		int WindowMenuButtonPosition{ 0 };
		float MouseCursorScale{ 1.0f };

		// -------- 显示/抗锯齿 --------
		ImVec2 DisplayWindowPadding{ 20.8f, 20.0f };
		ImVec2 DisplaySafeAreaPadding{ 4.2f, 4.0f };
		bool AntiAliasedLines{ true };
		bool AntiAliasedFill{ true };
		float CurveTessellationMaxError{ 0.5f };
		float CircleTessellationMaxError{ 0.4f };
	};

	class EditorStyle
	{
	public:
		// 主题列表：当前仅保留默认深色奢华主题，未来新增主题时在此追加枚举值
		enum class ETheme : int
		{
			DarkLuxury = 0, // 深色奢华(默认)
			Count
		};

	public:
		static void SetDefaultStyle();                 // 加载字体并应用默认主题(仅调用一次)
		static void ApplyTheme(ETheme theme);          // 应用主题配色 + 形状布局
		static ETheme GetCurrentTheme();               // 当前主题
		static const char* GetThemeName(ETheme theme); // 主题显示名

		// ---- 运行时外观编辑 ----
		static EditorStyleSettings& GetSettings();      // 可运行时修改的设置(修改后调用 ApplyCurrentStyle 生效)
		static void ApplyCurrentStyle();                // 用当前设置重新应用(滑块拖动时调用实现实时预览)
		static void ResetToDefaults();                  // 恢复默认并应用
		static void SaveConfig(const std::string& path); // 保存外观到 json 文件
		static void LoadConfig(const std::string& path); // 从 json 加载外观(无文件/解析失败回退默认)
		static const char* GetConfigPath();              // 默认配置文件路径(Config/Style.json)

	private:
		static void ApplyLayout(const EditorStyleSettings& s);
		static void ApplyPalette(const EditorStyleSettings& s);
	};

	REGISTER_TYPE(EditorStyleSettings);

	// 颜色
	REGISTER_MEMBER(EditorStyleSettings, Bg0);
	REGISTER_MEMBER(EditorStyleSettings, Bg1);
	REGISTER_MEMBER(EditorStyleSettings, Bg2);
	REGISTER_MEMBER(EditorStyleSettings, Hovered);
	REGISTER_MEMBER(EditorStyleSettings, Active);
	REGISTER_MEMBER(EditorStyleSettings, Border);
	REGISTER_MEMBER(EditorStyleSettings, Text);
	REGISTER_MEMBER(EditorStyleSettings, TextDisabled);
	REGISTER_MEMBER(EditorStyleSettings, Accent);

	// 间距/内边距
	REGISTER_MEMBER(EditorStyleSettings, WindowPadding);
	REGISTER_MEMBER(EditorStyleSettings, FramePadding);
	REGISTER_MEMBER(EditorStyleSettings, CellPadding);
	REGISTER_MEMBER(EditorStyleSettings, ItemSpacing);
	REGISTER_MEMBER(EditorStyleSettings, ItemInnerSpacing);
	REGISTER_MEMBER(EditorStyleSettings, IndentSpacing);
	REGISTER_MEMBER(EditorStyleSettings, ScrollbarSize);
	REGISTER_MEMBER(EditorStyleSettings, ScrollbarPadding);
	REGISTER_MEMBER(EditorStyleSettings, GrabMinSize);

	// 边框
	REGISTER_MEMBER(EditorStyleSettings, WindowBorderSize);
	REGISTER_MEMBER(EditorStyleSettings, ChildBorderSize);
	REGISTER_MEMBER(EditorStyleSettings, PopupBorderSize);
	REGISTER_MEMBER(EditorStyleSettings, FrameBorderSize);
	REGISTER_MEMBER(EditorStyleSettings, TabBorderSize);
	REGISTER_MEMBER(EditorStyleSettings, TabBarBorderSize);
	REGISTER_MEMBER(EditorStyleSettings, TabBarOverlineSize);
	REGISTER_MEMBER(EditorStyleSettings, DockingSeparatorSize);
	REGISTER_MEMBER(EditorStyleSettings, WindowBorderHoverPadding);
	REGISTER_MEMBER(EditorStyleSettings, SeparatorSize);

	// 圆角
	REGISTER_MEMBER(EditorStyleSettings, WindowRounding);
	REGISTER_MEMBER(EditorStyleSettings, ChildRounding);
	REGISTER_MEMBER(EditorStyleSettings, FrameRounding);
	REGISTER_MEMBER(EditorStyleSettings, PopupRounding);
	REGISTER_MEMBER(EditorStyleSettings, ScrollbarRounding);
	REGISTER_MEMBER(EditorStyleSettings, GrabRounding);
	REGISTER_MEMBER(EditorStyleSettings, TabRounding);
	REGISTER_MEMBER(EditorStyleSettings, MenuItemRounding);
	REGISTER_MEMBER(EditorStyleSettings, ImageRounding);
	REGISTER_MEMBER(EditorStyleSettings, ImageBorderSize);
	REGISTER_MEMBER(EditorStyleSettings, TreeLinesSize);
	REGISTER_MEMBER(EditorStyleSettings, TreeLinesRounding);
	REGISTER_MEMBER(EditorStyleSettings, DragDropTargetRounding);
	REGISTER_MEMBER(EditorStyleSettings, DragDropTargetBorderSize);
	REGISTER_MEMBER(EditorStyleSettings, DragDropTargetPadding);

	// 文本对齐/装饰
	REGISTER_MEMBER(EditorStyleSettings, WindowTitleAlign);
	REGISTER_MEMBER(EditorStyleSettings, ButtonTextAlign);
	REGISTER_MEMBER(EditorStyleSettings, SelectableTextAlign);
	REGISTER_MEMBER(EditorStyleSettings, WindowMenuButtonPosition);
	REGISTER_MEMBER(EditorStyleSettings, MouseCursorScale);

	// 显示/抗锯齿
	REGISTER_MEMBER(EditorStyleSettings, DisplayWindowPadding);
	REGISTER_MEMBER(EditorStyleSettings, DisplaySafeAreaPadding);
	REGISTER_MEMBER(EditorStyleSettings, AntiAliasedLines);
	REGISTER_MEMBER(EditorStyleSettings, AntiAliasedFill);
	REGISTER_MEMBER(EditorStyleSettings, CurveTessellationMaxError);
	REGISTER_MEMBER(EditorStyleSettings, CircleTessellationMaxError);
}
