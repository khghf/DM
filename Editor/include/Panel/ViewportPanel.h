#pragma once
#include "Panel.h"
#include <Framework/Camera/CameraController.h>
#include <Core/Render/FrameBuffer.h>
#include <functional>
#include <filesystem>

namespace DM
{
	struct ViewPort
	{
		Vector2 MinBound = { 0.f,0.f };//视口左下角
		Vector2 MaxBound = { 0.f,0.f };//视口右上角
		Vector2 Size = { 0.f,0.f };
		Vector2 MousePosLocal = { 0.f,0.f };//相对与视口左下角的坐标
		Vector2 MousePosScreen = { 0.f,0.f };//相对与屏幕左上角的坐标
		Vector2 MousePosLocalNarmal = { 0.f,0.f };
		bool bFocused = false;
		bool bHovered = false;
		void Reset()
		{
			MinBound = { 0.f,0.f };
			MaxBound = { 0.f,0.f };
			Size = { 0.f,0.f };
			MousePosLocal = { 0.f,0.f };
			MousePosScreen = { 0.f,0.f };
			MousePosLocalNarmal = { 0.f,0.f };
			bFocused = false;
			bHovered = false;
		}
	};

	class ViewportPanel : public Panel
	{
		friend class EditorLayer;
		ViewportPanel(const SPtr<World>& context, float windowWidth, float windowHeight);
	public:
		void OnUpdate(float DeltaTime);
		void OnEvent(Event* const e);
		// 拖拽打开场景的回调，由 EditorLayer 设置
		std::function<void(std::filesystem::path)> OnOpenScene;
	protected:
		virtual void Render() override;
		void RenderViewPort();
		void RenderSetting();
	private:
		CameraController m_CameraController;
		SPtr<FrameBuffer> m_ViewportFramebuffer;
		ViewPort m_ViewPort;
		Vector2 m_PendingViewportSize = { 0.f, 0.f };
	};
}
