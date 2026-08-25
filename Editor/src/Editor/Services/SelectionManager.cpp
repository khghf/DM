#include <DMPCH.h>
#include "Editor/Services/SelectionManager.h"
#include <algorithm>

namespace DM
{
	namespace
	{
		// 比较实体句柄(忽略 World 指针；Entity 的 operator== 非 const，无法用于 const 引用)
		bool SameEntity(const Entity& a, const Entity& b)
		{
			return static_cast<entt::entity>(a) == static_cast<entt::entity>(b);
		}
	}

	// ==================== 实体选中 ====================

	Entity SelectionManager::GetPrimaryEntity() const
	{
		return m_Entities.empty() ? Entity() : m_Entities.back();
	}

	bool SelectionManager::ContainsEntity(const Entity& entity) const
	{
		for (const Entity& e : m_Entities)
		{
			if (SameEntity(e, entity)) return true;
		}
		return false;
	}

	void SelectionManager::SetSelectedEntity(const Entity& entity)
	{
		m_Entities.clear();
		if (entity)
		{
			m_Entities.push_back(entity);
		}
		m_EntityAnchor = entity;
	}

	void SelectionManager::ToggleEntity(const Entity& entity)
	{
		for (auto it = m_Entities.begin(); it != m_Entities.end(); ++it)
		{
			if (SameEntity(*it, entity))
			{
				m_Entities.erase(it);
				m_EntityAnchor = entity;
				return;
			}
		}
		m_Entities.push_back(entity);
		m_EntityAnchor = entity;
	}

	void SelectionManager::SelectEntityRange(const std::vector<Entity>& orderedList, const Entity& clicked)
	{
		if (!m_EntityAnchor || orderedList.empty())
		{
			SetSelectedEntity(clicked);
			return;
		}

		auto itAnchor = orderedList.end();
		auto itClicked = orderedList.end();
		for (auto it = orderedList.begin(); it != orderedList.end(); ++it)
		{
			if (SameEntity(*it, m_EntityAnchor)) itAnchor = it;
			if (SameEntity(*it, clicked)) itClicked = it;
		}
		if (itAnchor == orderedList.end() || itClicked == orderedList.end())
		{
			SetSelectedEntity(clicked);
			return;
		}

		const size_t lo = std::min(itAnchor - orderedList.begin(), itClicked - orderedList.begin());
		const size_t hi = std::max(itAnchor - orderedList.begin(), itClicked - orderedList.begin());
		m_Entities.clear();
		for (size_t i = lo; i <= hi; ++i)
		{
			m_Entities.push_back(orderedList[i]);
		}

		// 点击项作为主选中(移至末尾)；锚点保持为原锚点，便于连续 Shift 扩展
		for (auto it = m_Entities.begin(); it != m_Entities.end(); ++it)
		{
			if (SameEntity(*it, clicked) && (it + 1) != m_Entities.end())
			{
				Entity primary = *it;
				m_Entities.erase(it);
				m_Entities.push_back(primary);
				break;
			}
		}
	}

	void SelectionManager::AddEntity(const Entity& entity)
	{
		if (!entity || ContainsEntity(entity)) return;
		m_Entities.push_back(entity);
	}

	void SelectionManager::RemoveEntity(const Entity& entity)
	{
		for (auto it = m_Entities.begin(); it != m_Entities.end(); ++it)
		{
			if (SameEntity(*it, entity))
			{
				m_Entities.erase(it);
				return;
			}
		}
	}

	void SelectionManager::ClearEntities()
	{
		m_Entities.clear();
		m_EntityAnchor = Entity();
	}

	// ==================== 文件/目录路径选中 ====================

	const std::string& SelectionManager::GetPrimaryPath() const
	{
		static const std::string empty;
		return m_Paths.empty() ? empty : m_Paths.back();
	}

	bool SelectionManager::ContainsPath(const std::string& path) const
	{
		return std::find(m_Paths.begin(), m_Paths.end(), path) != m_Paths.end();
	}

	void SelectionManager::SetSelectedPath(const std::string& path)
	{
		m_Paths.clear();
		if (!path.empty())
		{
			m_Paths.push_back(path);
		}
		m_PathAnchor = path;
	}

	void SelectionManager::TogglePath(const std::string& path)
	{
		auto it = std::find(m_Paths.begin(), m_Paths.end(), path);
		if (it != m_Paths.end())
		{
			m_Paths.erase(it);
		}
		else
		{
			m_Paths.push_back(path);
		}
		m_PathAnchor = path;
	}

	void SelectionManager::SelectPathRange(const std::vector<std::string>& orderedList, const std::string& clicked)
	{
		if (m_PathAnchor.empty() || orderedList.empty())
		{
			SetSelectedPath(clicked);
			return;
		}

		auto itAnchor = std::find(orderedList.begin(), orderedList.end(), m_PathAnchor);
		auto itClicked = std::find(orderedList.begin(), orderedList.end(), clicked);
		if (itAnchor == orderedList.end() || itClicked == orderedList.end())
		{
			SetSelectedPath(clicked);
			return;
		}

		const size_t lo = std::min(itAnchor - orderedList.begin(), itClicked - orderedList.begin());
		const size_t hi = std::max(itAnchor - orderedList.begin(), itClicked - orderedList.begin());
		m_Paths.clear();
		for (size_t i = lo; i <= hi; ++i)
		{
			m_Paths.push_back(orderedList[i]);
		}

		// 点击项作为主选中(移至末尾)；锚点保持为原锚点
		for (auto it = m_Paths.begin(); it != m_Paths.end(); ++it)
		{
			if (*it == clicked && (it + 1) != m_Paths.end())
			{
				std::string primary = *it;
				m_Paths.erase(it);
				m_Paths.push_back(primary);
				break;
			}
		}
	}

	void SelectionManager::AddPath(const std::string& path)
	{
		if (path.empty() || ContainsPath(path)) return;
		m_Paths.push_back(path);
	}

	void SelectionManager::RemovePath(const std::string& path)
	{
		auto it = std::find(m_Paths.begin(), m_Paths.end(), path);
		if (it != m_Paths.end())
		{
			m_Paths.erase(it);
		}
	}

	void SelectionManager::ClearPaths()
	{
		m_Paths.clear();
		m_PathAnchor.clear();
	}

	// ==================== 通用 ====================

	void SelectionManager::ClearAll()
	{
		m_Entities.clear();
		m_EntityAnchor = Entity();
		m_Paths.clear();
		m_PathAnchor.clear();
	}
}
