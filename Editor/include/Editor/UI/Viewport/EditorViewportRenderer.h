#pragma once
#include <imgui.h>

namespace DM
{
	/// <summary>
	/// 视口渲染服务：把引擎渲染帧缓冲输出到 ImGui 纹理。
	/// </summary>
	class EditorViewportRenderer
	{
	public:
		EditorViewportRenderer();
		~EditorViewportRenderer();

		/// <summary>
		/// 获取当前帧缓冲对应的 ImGui 纹理引用(每帧调用，内部惰性更新)
		/// </summary>
		ImTextureRef GetViewportTextureRef();

	private:
		void UpdateTextureIfNeeded();

	private:
		ImTextureRef m_TextureRef{};
	};
}
