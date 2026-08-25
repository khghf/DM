#pragma once
#include<functional>
#include<string>
#include<vector>
#include<imgui.h>
namespace DM
{
	
	class EditorShortcuts
	{
	public:
		using Chord = ImGuiKeyChord;
		using Action = std::function<void()>;

		static EditorShortcuts* Get()
		{
			static EditorShortcuts inst;
			return &inst;
		}

		/// <summary>
		/// 注册快捷键：同一 chord 可多次注册(按注册顺序依次触发)。
		/// flags 默认 ImGuiInputFlags_RouteGlobal(编辑器级全局路由)：Process 在 ImGui 新帧之后、无窗口上下文时调用，
		/// 因此不能依赖 RouteFocused 焦点路由；文本输入控件(InputText)活跃时 Process 会自动跳过全局快捷键让其优先消费编辑键。
		/// 若需要在窗口/控件上下文内调用，可显式传 ImGuiInputFlags_RouteFocused。
		/// </summary>
		void Register(Chord chord, Action action, std::string name, ImGuiInputFlags flags = ImGuiInputFlags_RouteGlobal);
		/// <summary>按名称移除快捷键</summary>
		void Unregister(const std::string& name);
		/// <summary>每帧在 ImGui 新帧之后调用：检测所有快捷键并触发对应动作</summary>
		void Process();

		/// <summary>集中注册编辑器默认快捷键(编辑器 OnAttach 时调用一次)</summary>
		void RegisterDefaults();

		/// <summary>将字符串快捷键(如 "Ctrl+O")解析为 ImGuiKeyChord；无效时返回 0</summary>
		static ImGuiKeyChord ParseShortcut(const std::string& text);

	private:
		struct Entry
		{
			Chord Chord;
			ImGuiInputFlags Flags = ImGuiInputFlags_RouteGlobal;
			Action Action;
			std::string Name;
		};
		std::vector<Entry> m_Entries;
	};
}
