#pragma once
#include<Core/AssetManagent/AssetObject/AssetObject.h>
#include<entt/entt.hpp>
namespace DM
{
	class Entity;
	class DM_API World:public AssetObject
	{
		friend class Entity;
		friend class WorldSerializer;   // 场景序列化器需要访问 m_Registry 做实体快照
#ifdef  DM_EDITOR
		friend class Editor;
		friend class SceneHierarchyPanel;
		friend class ViewportPanel;
		friend class MenuBarPanel;
#endif //  DM_EDITOR
	public:
		World()=default;
		~World();
	public:
		Entity CreateEntity();
		void DestroyEntity(Entity entity);
		std::string m_Name = "Unnamed";
		std::string m_path;
		bool bHasSavedToLocal = false;//在本地序列化过

		// ==================== 修改状态(切换世界前保存判定) ====================
		bool bDirty = false;                      // 是否有未保存修改：实体增删/组件变更后置位，保存后清零
		void MarkDirty() { bDirty = true; }
		void ClearDirty() { bDirty = false; }
		bool IsDirty() const { return bDirty; }
		constexpr static std::string_view s_FileExtension = "world";


		virtual EAssetType		GetResourceType()const { return GetAssetType_Static(); }
		static EAssetType		GetAssetType_Static() { return EAssetType::World; }
		virtual std::string		GetExtension()const		 { return ".world"; }

		const entt::registry&	GetEnttRegistry()const { return m_Registry; }
	private:
		entt::registry m_Registry;
	};
}
