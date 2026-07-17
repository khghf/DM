#pragma once
// ============================================================
// ImGuiCompat.h - 兼容性垫片
//
// 当 ImGui 构建时未启用 IMGUI_HAS_DOCK (如 ImGuizmo 捆绑版),
// 将所有视口/停靠相关的枚举常量定义为宏(值为 0),
// 使得依赖这些常量的代码可以编译通过(功能静默禁用)。
// ============================================================

#include <imgui.h>

#ifndef IMGUI_HAS_DOCK
	#define ImGuiConfigFlags_DockingEnable         0
	#define ImGuiConfigFlags_ViewportsEnable       0
	#define ImGuiBackendFlags_PlatformHasViewports 0
	#define ImGuiBackendFlags_RendererHasViewports 0
	#define ImGuiBackendFlags_HasMouseHoveredViewport 0
	#define ImGuiViewportFlags_NoDecoration        0
	#define ImGuiViewportFlags_NoInputs            0
	#define ImGuiViewportFlags_TopMost             0
	#define ImGuiViewportFlags_NoRendererClear     0
	#define ImGuiViewportFlags_NoTaskBarIcon       0
#endif
