#pragma once
#include<Core/AssetManagent/AssetObject/AssetObject.h>
#include<entt/entt.hpp>
namespace DM
{
	class Entity;
	class DM_API World:public AssetObject
	{
		friend class Entity;
#ifdef  DM_EDITOR
		friend class Editor;
		friend class SceneHierarchyPanel;
		friend class ViewportPanel;
		friend class Serializer;
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
		constexpr static std::string_view s_FileExtension = "world";


		virtual EAssetType		GetResourceType()const { return GetAssetType_Static(); }
		static EAssetType		GetAssetType_Static() { return EAssetType::World; }
		virtual std::string		GetExtension()const		 { return ".world"; }

		const entt::registry&	GetEnttRegistry()const { return m_Registry; }
	private:
		entt::registry m_Registry;
	};
}
