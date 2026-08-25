#include "Editor/Services/EditorClipboard.h"
#include <filesystem>

namespace DM
{
	void EditorClipboard::Set(const std::vector<std::string>& paths, EMode mode)
	{
		m_Paths = paths;
		m_Mode = mode;
	}

	void EditorClipboard::Clear()
	{
		m_Paths.clear();
		m_Mode = EMode::None;
	}

	bool EditorClipboard::Contains(const std::string& absPath) const
	{
		const std::filesystem::path p = std::filesystem::path(absPath).lexically_normal();
		for (const std::string& s : m_Paths)
		{
			if (std::filesystem::path(s).lexically_normal() == p) return true;
		}
		return false;
	}

	EditorClipboard::Snapshot EditorClipboard::SnapshotState() const
	{
		Snapshot snap;
		snap.Mode = m_Mode;
		snap.Paths = m_Paths;
		return snap;
	}

	void EditorClipboard::Restore(const Snapshot& snap)
	{
		m_Mode = snap.Mode;
		m_Paths = snap.Paths;
	}
}
