#pragma once
#include "Framework/Base/Entity.h"
#include "Editor/Core/Service/IService.h"
#include <string>
#include <vector>

namespace DM
{
	/// <summary>
	/// 编辑器统一选中管理：实体/文件/目录三类对象的选中状态与多选交互全部集中于此。
	/// </summary>
	class SelectionManager : public IService
	{
	public:
		SelectionManager() = default;

		const std::vector<Entity>& GetSelectedEntities() const { return m_Entities; }
		/// <summary>主选中实体(最后点击/恢复的实体)，供属性面板等单实体消费方使用</summary>
		Entity	GetPrimaryEntity() const;
		bool	HasSelectedEntity() const { return !m_Entities.empty(); }
		bool	ContainsEntity(const Entity& entity) const;

		/// <summary>单选：清空后选中该实体，并将其设为范围选择锚点</summary>
		void	SetSelectedEntity(const Entity& entity);
		/// <summary>Ctrl+点击</summary>
		void	ToggleEntity(const Entity& entity);

		/// <summary>Shift+点击</summary>
		void	SelectEntityRange(const std::vector<Entity>& orderedList, const Entity& clicked);
		void	AddEntity(const Entity& entity);
		void	RemoveEntity(const Entity& entity);
		void	ClearEntities();

		// ==================== 文件/目录路径选中 ====================
		const std::vector<std::string>& GetSelectedPaths() const { return m_Paths; }
		/// <summary>主选中路径(最后点击/恢复的路径)</summary>
		const std::string&	GetPrimaryPath() const;
		bool				HasSelectedPath() const { return !m_Paths.empty(); }
		bool				ContainsPath(const std::string& path) const;

		/// <summary>单选：清空后选中该路径，并将其设为范围选择锚点</summary>
		void				SetSelectedPath(const std::string& path);
		/// <summary>Ctrl+点击：在选中集合中切换该路径的选中状态</summary>
		void				TogglePath(const std::string& path);
		/// <summary>Shift+点击：按锚点↔点击项在有序列表中的区间做范围选择</summary>
		void				SelectPathRange(const std::vector<std::string>& orderedList, const std::string& clicked);
		void				AddPath(const std::string& path);
		void				RemovePath(const std::string& path);
		void				ClearPaths();

		/// <summary>清空实体与路径全部选中</summary>
		void ClearAll();

	private:
		std::vector<Entity> m_Entities;      // 实体多选集合
		Entity m_EntityAnchor;               // 实体范围选择锚点
		std::vector<std::string> m_Paths;    // 文件/目录多选集合(完整路径)
		std::string m_PathAnchor;            // 路径范围选择锚点
	};
}
