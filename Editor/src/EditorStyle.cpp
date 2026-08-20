#include <EditorStyle.h>
#include<imgui.h>
#include<string>
#include<Core/AssetManagent/AssetMgr.h>
namespace DM
{
	void EditorStyle::SetDefaultStyle()
	{
		ImGuiIO&io=ImGui::GetIO();
		std::string Neuton_Bold = AssetRootDir + "/Font/Neuton/Neuton-Bold.ttf";
		std::string Neuton_Regular = AssetRootDir + "/Font/Neuton/Neuton-Regular.ttf";
		io.Fonts->AddFontFromFileTTF(Neuton_Bold.c_str(), 20.f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF(Neuton_Regular.c_str(), 20.f);
		SetupImGuiDarkLuxuryTheme();
	}

	void EditorStyle::SetupImGuiDarkLuxuryTheme(float alpha)
	{
		// 获取ImGui全局样式对象：所有UI的颜色、布局、圆角等参数都存储在此对象中
		ImGuiStyle& style = ImGui::GetStyle();
		// 获取颜色数组指针：ImGuiCol_XXX枚举对应不同UI元素的颜色索引，通过指针直接修改颜色
		ImVec4* colors = style.Colors;

		// ================ 核心配色定义：最初的高级黑色系（带亮度层次） ================
		// 背景色层级（从深到浅）：保证视觉区分度，避免纯黑压抑，保留高级质感
		const ImVec4 bg_primary = ImVec4(0.08f, 0.08f, 0.08f, alpha); // 主背景（窗口底色）：深黑带轻微灰度，避免纯黑(0,0,0)的视觉压抑
		const ImVec4 bg_secondary = ImVec4(0.12f, 0.12f, 0.12f, alpha); // 次级背景（弹窗/菜单栏）：略亮于主背景，区分独立功能区域
		const ImVec4 bg_tertiary = ImVec4(0.18f, 0.18f, 0.18f, alpha); // 控件背景（按钮/输入框/滑块）：更亮的黑，突出控件主体
		// 交互态颜色：通过灰度提升实现反馈，不破坏黑色系整体风格
		const ImVec4 highlight = ImVec4(0.35f, 0.35f, 0.35f, alpha); // 悬浮态：鼠标划过控件的背景色，灰度更高
		const ImVec4 active = ImVec4(0.45f, 0.45f, 0.45f, alpha); // 激活态：点击/选中/拖动控件的背景色，最亮的交互态
		// 辅助色：保证控件边界和文本的可读性
		const ImVec4 border = ImVec4(0.20f, 0.20f, 0.20f, alpha); // 边框色：浅黑，勾勒控件/窗口边界，不突兀
		const ImVec4 text = ImVec4(0.90f, 0.90f, 0.90f, alpha); // 普通文本：亮灰，与黑色背景形成足够对比度，保证可读性
		const ImVec4 text_disabled = ImVec4(0.50f, 0.50f, 0.50f, alpha); // 禁用文本：中灰，区分不可交互状态

		// ================ 逐UI元素设置颜色（按功能分类，便于维护） ================
		// ---------------- 文本相关 ----------------
		colors[ImGuiCol_Text] = text;                  // 普通可交互文本（如按钮文字、标签）
		colors[ImGuiCol_TextDisabled] = text_disabled;         // 禁用状态文本（如灰掉的按钮文字）
		colors[ImGuiCol_TextSelectedBg] = highlight;             // 选中文本的背景色（如输入框选中文本、列表选中项）

		// ---------------- 窗口/容器相关 ----------------
		colors[ImGuiCol_WindowBg] = bg_primary;            // 主窗口背景（最外层容器）
		colors[ImGuiCol_ChildBg] = bg_primary;            // 子窗口/子面板背景（与主窗口一致，保持视觉统一）
		colors[ImGuiCol_PopupBg] = bg_secondary;          // 弹窗背景（略亮，区分弹窗与主窗口）
		colors[ImGuiCol_MenuBarBg] = bg_secondary;          // 菜单栏背景（与弹窗一致，区分功能区）
		colors[ImGuiCol_TitleBg] = bg_primary;            // 窗口标题栏常态背景
		colors[ImGuiCol_TitleBgActive] = bg_secondary;          // 窗口激活时标题栏背景（略亮，反馈窗口激活状态）
		colors[ImGuiCol_TitleBgCollapsed] = bg_primary;            // 窗口折叠时标题栏背景（与常态一致）

		// ---------------- 边框/分隔线相关 ----------------
		colors[ImGuiCol_Border] = border;                // 窗口/控件的边框色
		colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, alpha); // 边框阴影：纯黑，弱化阴影效果，避免脏感
		colors[ImGuiCol_Separator] = border;                // 分隔线常态（如窗口内区域分隔）
		colors[ImGuiCol_SeparatorHovered] = highlight;             // 分隔线悬浮态（可拖动的分隔线）
		colors[ImGuiCol_SeparatorActive] = active;                // 分隔线激活态（拖动中的分隔线）

		// ---------------- 基础控件背景（输入框/按钮/滑块等） ----------------
		colors[ImGuiCol_FrameBg] = bg_tertiary;           // 控件框架背景（输入框、复选框、滑块底座、下拉框等）
		colors[ImGuiCol_FrameBgHovered] = highlight;             // 控件框架悬浮态
		colors[ImGuiCol_FrameBgActive] = active;                // 控件框架激活态（点击/输入中）

		// ---------------- 按钮/表头相关 ----------------
		colors[ImGuiCol_Button] = bg_tertiary;           // 按钮常态背景
		colors[ImGuiCol_ButtonHovered] = highlight;             // 按钮悬浮态
		colors[ImGuiCol_ButtonActive] = active;                // 按钮按下态
		colors[ImGuiCol_Header] = bg_tertiary;           // 表头/折叠面板头常态
		colors[ImGuiCol_HeaderHovered] = highlight;             // 表头/折叠面板头悬浮态
		colors[ImGuiCol_HeaderActive] = active;                // 表头/折叠面板头激活态（展开/选中）

		// ---------------- 滚动条相关 ----------------
		colors[ImGuiCol_ScrollbarBg] = bg_tertiary;           // 滚动条背景槽（未被滑块覆盖的区域）
		colors[ImGuiCol_ScrollbarGrab] = highlight;             // 滚动条滑块常态
		colors[ImGuiCol_ScrollbarGrabHovered] = active;                // 滚动条滑块悬浮态
		colors[ImGuiCol_ScrollbarGrabActive] = active;                // 滚动条滑块拖动态

		// ---------------- 交互标记/手柄相关 ----------------
		colors[ImGuiCol_CheckMark] = highlight;             // 复选框/单选框的勾选标记
		colors[ImGuiCol_SliderGrab] = highlight;             // 滑块手柄常态
		colors[ImGuiCol_SliderGrabActive] = active;                // 滑块手柄拖动态
		colors[ImGuiCol_ResizeGrip] = bg_tertiary;           // 窗口调整手柄常态（窗口右下角）
		colors[ImGuiCol_ResizeGripHovered] = highlight;             // 调整手柄悬浮态
		colors[ImGuiCol_ResizeGripActive] = active;                // 调整手柄拖动态

		// ---------------- 标签页相关 ----------------
		colors[ImGuiCol_Tab] = bg_tertiary;           // 标签常态（未选中）
		colors[ImGuiCol_TabHovered] = highlight;             // 标签悬浮态
		colors[ImGuiCol_TabActive] = active;                // 标签激活态（当前选中）
		colors[ImGuiCol_TabUnfocused] = bg_secondary;          // 未聚焦窗口的标签常态
		colors[ImGuiCol_TabUnfocusedActive] = bg_tertiary;           // 未聚焦窗口的激活标签

		// ---------------- 图表相关 ----------------
		colors[ImGuiCol_PlotLines] = highlight;             // 折线图线条常态
		colors[ImGuiCol_PlotLinesHovered] = active;                // 折线图线条悬浮态（鼠标划过数据点）
		colors[ImGuiCol_PlotHistogram] = highlight;             // 直方图常态
		colors[ImGuiCol_PlotHistogramHovered] = active;                // 直方图悬浮态

		// ---------------- 表格相关 ----------------
		colors[ImGuiCol_TableHeaderBg] = bg_secondary;          // 表格表头背景
		colors[ImGuiCol_TableBorderStrong] = border;                // 表格强边框（外框、表头分隔线）
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.15f, 0.15f, 0.15f, alpha); // 表格弱边框（单元格分隔线）
		colors[ImGuiCol_TableRowBg] = bg_primary;            // 表格行背景（默认行）
		colors[ImGuiCol_TableRowBgAlt] = bg_secondary;          // 表格交替行背景（隔行变色，提升可读性）

		// ---------------- 拖放/导航相关 ----------------
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.4f, 0.4f, 0.4f, 0.8f * alpha); // 拖放目标高亮背景（降低透明度，突出反馈）
		colors[ImGuiCol_NavHighlight] = active;                // 键盘/手柄导航高亮（选中的控件）
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f * alpha); // 窗口导航高亮（Alt+Tab切换窗口时）
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.2f * alpha); // 窗口导航暗背景（非当前窗口）
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.35f * alpha); // 模态窗口暗背景（遮挡下层内容）

		// ================ 样式布局微调：增强高级黑的视觉质感 ================
		// ---------------- 内边距：保证控件间距合理，避免拥挤 ----------------
		style.WindowPadding = ImVec2(8.0f, 8.0f);    // 窗口内边距：窗口边缘到内容的距离
		style.FramePadding = ImVec2(4.0f, 4.0f);    // 控件内边距：控件边缘到内容/文字的距离
		style.CellPadding = ImVec2(4.0f, 2.0f);    // 表格单元格内边距
		style.ItemSpacing = ImVec2(8.0f, 4.0f);    // 控件之间的间距（横向+纵向）
		style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);    // 复合控件内部间距（如按钮组、多选框+文字）
		style.TouchExtraPadding = ImVec2(0.0f, 0.0f);    // 触摸操作额外内边距（适配触屏设备，此处禁用）
		style.IndentSpacing = 20.0f;                 // 缩进间距（如树节点、列表项的缩进）
		style.ScrollbarSize = 12.0f;                 // 滚动条宽度（默认尺寸，保证精致感）
		style.GrabMinSize = 10.0f;                 // 手柄最小尺寸（滑块/滚动条手柄，避免过小难以点击）

		// ---------------- 边框尺寸：简约风格，取消控件边框 ----------------
		style.WindowBorderSize = 1.0f;                  // 窗口边框宽度
		style.ChildBorderSize = 1.0f;                  // 子窗口边框宽度
		style.PopupBorderSize = 1.0f;                  // 弹窗边框宽度
		style.FrameBorderSize = 0.0f;                  // 控件框架边框宽度：取消边框，用背景色区分更简约
		style.TabBorderSize = 0.0f;                  // 标签边框宽度：取消边框，保持风格统一

		// ---------------- 圆角：保留圆润质感，避免直角生硬 ----------------
		style.WindowRounding = 4.0f;                  // 窗口圆角半径
		style.ChildRounding = 4.0f;                  // 子窗口圆角半径
		style.FrameRounding = 2.0f;                  // 控件框架圆角半径
		style.PopupRounding = 4.0f;                  // 弹窗圆角半径
		style.ScrollbarRounding = 8.0f;                  // 滚动条圆角半径
		style.GrabRounding = 4.0f;                  // 手柄圆角半径
		style.TabRounding = 4.0f;                  // 标签圆角半径

		// ---------------- 文本对齐：优化视觉居中体验 ----------------
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);    // 窗口标题文字：水平+垂直居中
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);    // 按钮文字：水平+垂直居中
		style.SelectableTextAlign = ImVec2(0.0f, 0.0f);    // 可选文本（列表项）：左对齐

		// ---------------- 显示相关：适配不同屏幕/操作方式 ----------------
		style.DisplayWindowPadding = ImVec2(20.0f, 20.0f);  // 多窗口显示时的内边距（避免窗口贴边）
		style.DisplaySafeAreaPadding = ImVec2(4.0f, 4.0f);    // 屏幕安全区域内边距（适配刘海屏/全面屏）
		style.MouseCursorScale = 1.0f;                  // 鼠标光标缩放比例（默认1.0）
		// ---------------- 抗锯齿：提升视觉细腻度 ----------------
		style.AntiAliasedLines = true;                  // 线条抗锯齿（边框、分隔线、图表线条）：开启后更顺滑
		style.AntiAliasedFill = true;                  // 填充抗锯齿（按钮背景、圆角区域）：开启后更细腻
		style.CurveTessellationTol = 1.25f;                 // 曲线细分精度：越高曲线越平滑，性能影响可忽略
	}


}