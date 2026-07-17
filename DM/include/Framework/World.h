#pragma once
#include"Object.h"
namespace DM
{
	class Entity;
	class DM_API CLASS() World:public Object
	{
		friend class Entity;
		friend class EditorLayer;
		friend class SceneHierarchyPanel;
		friend class ViewportPanel;
		friend class Serializer;
		World();
	public:
		~World();
	public:
		Entity CreateEntity();
		void DestroyEntity(Entity entity);
		std::string m_Name = "Unnamed";
		std::string m_path;
		bool bHasSavedToLocal = false;//在本地序列化过
		constexpr static std::string_view s_FileExtension = "world";
	private:
		entt::registry m_Registry;
	};
}
