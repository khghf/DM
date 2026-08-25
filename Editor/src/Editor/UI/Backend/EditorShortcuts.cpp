#include <Editor/Commands/CommandHistory.h>
#include <Editor/Commands/CommandInvoker.h>
#include <Editor/UI/Backend/EditorShortcuts.h>
#include <Editor/Commands/CommandRegistry.h>
#include <Editor/Core/EditorContext.h>
#include <Editor/Services/SelectionManager.h>

namespace DM
{
	void EditorShortcuts::Register(Chord chord, Action action, std::string name, ImGuiInputFlags flags)
	{
		m_Entries.push_back(Entry{ chord, flags, std::move(action), std::move(name) });
	}

	void EditorShortcuts::Unregister(const std::string& name)
	{
		std::erase_if(m_Entries, [&](const Entry& e) { return e.Name == name; });
	}

	ImGuiKeyChord EditorShortcuts::ParseShortcut(const std::string& text)
	{
		ImGuiKeyChord chord = 0;
		std::string key;
		size_t pos = 0;
		while (pos <= text.size())
		{
			const size_t plus = text.find('+', pos);
			const std::string token = (plus == std::string::npos)? text.substr(pos): text.substr(pos, plus - pos);

			if		(token == "Ctrl")			chord |= ImGuiMod_Ctrl;
			else if (token == "Shift")			chord |= ImGuiMod_Shift;
			else if (token == "Alt")			chord |= ImGuiMod_Alt;
			else if (token == "Super")			chord |= ImGuiMod_Super;
			else key = token;

			if (plus == std::string::npos) break;

			pos = plus + 1;
		}
		if (key.empty()) return 0;

		// 字母 / 数字
		if (key.size() == 1)
		{
			const char c = key[0];
			if (c >= 'a' && c <= 'z') return chord | static_cast<ImGuiKeyChord>(ImGuiKey_A + (c - 'a'));
			if (c >= 'A' && c <= 'Z') return chord | static_cast<ImGuiKeyChord>(ImGuiKey_A + (c - 'A'));
			if (c >= '0' && c <= '9') return chord | static_cast<ImGuiKeyChord>(ImGuiKey_0 + (c - '0'));
		}

		// 命名键
		struct NamedKey { const char* Name; ImGuiKey Key; };
		static const NamedKey named[] = {
			{ "Delete", ImGuiKey_Delete },	{ "Backspace",	ImGuiKey_Backspace },
			{ "Enter",	ImGuiKey_Enter },	{ "Tab",		ImGuiKey_Tab },
			{ "Space",	ImGuiKey_Space },	{ "Escape",		ImGuiKey_Escape },
			{ "F1",		ImGuiKey_F1 },		{ "F2",			ImGuiKey_F2 },			{ "F3",		ImGuiKey_F3 },
			{ "F4",		ImGuiKey_F4 },		{ "F5",			ImGuiKey_F5 },			{ "F6",		ImGuiKey_F6 },
			{ "F7",		ImGuiKey_F7 },		{ "F8",			ImGuiKey_F8 },			{ "F9",		ImGuiKey_F9 },
			{ "F10",	ImGuiKey_F10 },		{ "F11",		ImGuiKey_F11 },			{ "F12",	ImGuiKey_F12 },
		};
		for (const auto& n : named)
		{
			if (key == n.Name) return chord | static_cast<ImGuiKeyChord>(n.Key);
		}
		return 0;
	}

	namespace
	{
		// 判断快捷键是否属于"文本编辑类"——输入框(InputText)活跃时应让行：
		//  - Delete/Backspace/方向键/Home/End/PageUp/PageDown/Enter/Tab 等编辑导航键
		//  - 无修饰键的字符键(字母/数字/空格等，会向输入框输入字符)
		//  - InputText 内部的文本编辑组合(Ctrl+Z/Y/X/C/V/A：撤销/重做/剪切/复制/粘贴/全选)
		// 功能快捷键(Ctrl+S 保存、Ctrl+O 打开、F5 播放等)在输入框活跃时仍应生效，
		// 不能像旧逻辑那样"输入时一刀切屏蔽全部全局快捷键"导致 Ctrl+S 在输入框内无反应。
		bool IsEditingShortcut(ImGuiKeyChord chord)
		{
			const ImGuiKey key = (ImGuiKey)(chord & ~(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiMod_Alt | ImGuiMod_Super));
			switch (key)
			{
			case ImGuiKey_Delete:
			case ImGuiKey_Backspace:
			case ImGuiKey_LeftArrow:
			case ImGuiKey_RightArrow:
			case ImGuiKey_UpArrow:
			case ImGuiKey_DownArrow:
			case ImGuiKey_Home:
			case ImGuiKey_End:
			case ImGuiKey_PageUp:
			case ImGuiKey_PageDown:
			case ImGuiKey_Enter:
			case ImGuiKey_Tab:
				return true;
			default:
				break;
			}

			// 无修饰键的字符键(字母/数字/空格)会向输入框输入字符，须让行
			const bool hasModifier = (chord & (ImGuiMod_Ctrl | ImGuiMod_Alt | ImGuiMod_Super)) != 0;
			if (!hasModifier)
				return true;

			// 文本编辑组合键：InputText 内部用这些做撤销/重做/剪切/复制/粘贴/全选
			if ((chord & ImGuiMod_Ctrl) && key >= ImGuiKey_A && key <= ImGuiKey_Z)
			{
				switch (key)
				{
				case ImGuiKey_A:
				case ImGuiKey_C:
				case ImGuiKey_V:
				case ImGuiKey_X:
				case ImGuiKey_Y:
				case ImGuiKey_Z:
					return true;
				default:
					break;
				}
			}
			return false;
		}
	}

	void EditorShortcuts::Process()
	{
		// 文本输入控件(InputText 等)活跃时只屏蔽"文本编辑类"快捷键(Delete/方向键/字符键/Ctrl+Z/Y/X/C/V/A)，
		// 功能快捷键(Ctrl+S 保存、Ctrl+O 打开、F5 播放等)在输入时仍生效。
		const bool textInputActive = ImGui::GetIO().WantTextInput;

		for (const auto& entry : m_Entries)
		{
			if (!entry.Action)
				continue;
			if (textInputActive && IsEditingShortcut(entry.Chord))
				continue;
			if (ImGui::Shortcut(entry.Chord, entry.Flags))
			{
				entry.Action();
			}
		}
	}

	void EditorShortcuts::RegisterDefaults()
	{
		for (const EditorCommandSpec& cmd : CommandRegistry::Get().GetAll())
		{
			if (cmd.ShortcutText.empty()) continue;
			const ImGuiKeyChord chord = ParseShortcut(cmd.ShortcutText);
			if (chord == 0) continue;
			// 显式全局路由：ImGui::Shortcut 默认是 RouteFocused(仅聚焦窗口内且需 SetShortcutRouting 抢占)，
			// 面板均未抢占，窗口聚焦时快捷键会整体失灵；RouteGlobal 保证任意窗口/空白区域都触发，
			// 文本输入控件活跃时由 Process() 的 textInputActive 检查统一让行。
			Register(chord, cmd.Execute ? cmd.Execute : [] {}, "Cmd." + cmd.Id, ImGuiInputFlags_RouteGlobal);
		}

		// ============ 编辑 ============
		// 全部显式 RouteGlobal；文本输入控件活跃时 Process 自动让行(InputText 优先消费 Ctrl+Z/Y、Delete 等)
		Register(ImGuiMod_Ctrl | ImGuiKey_Z,	[] { CommandHistory::Get()->Undo(); },					"Editor.Undo",		ImGuiInputFlags_RouteGlobal);
		Register(ImGuiMod_Ctrl | ImGuiKey_Y,	[] { CommandHistory::Get()->Redo(); },					"Editor.Redo",		ImGuiInputFlags_RouteGlobal);
		Register(ImGuiKey_Delete,				[] {
			// 资产浏览器选中文件/目录时，Delete 由 ContentBrowserPanel 消费(删除资产)，此处直接让行，避免误删场景实体
			if (EditorContext::GetService<SelectionManager>()->HasSelectedPath())return;
			CommandInvoker::Invoke<CmdDeleteEntity>();
		},	"Editor.DeleteEntity",	ImGuiInputFlags_RouteGlobal);
		Register(ImGuiMod_Ctrl | ImGuiKey_D,	[] {
			// 资产浏览器选中文件/目录时，Ctrl+D 由 ContentBrowserPanel 消费(复制资产)，此处直接让行，避免在场景中凭空复制出实体副本
			if (EditorContext::GetService<SelectionManager>()->HasSelectedPath())return;
			CommandInvoker::Invoke<CmdDuplicateEntity>();
		},	"Editor.DuplicateEntity",	ImGuiInputFlags_RouteGlobal);

		// ============ 视口 ============
		Register(ImGuiKey_F, [] { /* TODO: 聚焦选中实体(视口相机未提供聚焦接口) */ },					"Editor.FocusSelection", ImGuiInputFlags_RouteGlobal);
	}
}
