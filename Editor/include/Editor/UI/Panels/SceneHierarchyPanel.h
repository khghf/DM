#pragma once
#include "Panel.h"
#include "Framework/Base/Entity.h"
#include <vector>
#include <string>
#include <entt/entt.hpp>

namespace DM
{
	class SceneHierarchyPanel : public Panel
	{
		friend class Editor;

		SceneHierarchyPanel() {};

	protected:
		virtual void Render() override;

	private:
		
		struct CachedEntity
		{
			entt::entity Handle;
			std::string Tag;
		};

		void RebuildCacheIfNeeded();
		const std::vector<CachedEntity>& GetFilteredEntities();
		void DrawEntityNode(const CachedEntity& cached);

	private:
	
		std::vector<CachedEntity> m_CachedEntities;      // 全部实体缓存
		std::vector<CachedEntity> m_FilteredEntities;    // 过滤后的实体
		bool m_CacheDirty = true;                        // 缓存脏标记
		char m_Filter[256] = {};                         // 搜索过滤
		uint64_t m_LastChangeCount = 0;                  // 上次刷新时的编辑器变更计数
	};
}
