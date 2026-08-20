#pragma once
#include"World.h"
namespace DM
{
	class DM_API Entity:public Object
	{
		friend class World;
		friend class SceneHierarchyPanel;
	public:
		Entity() = default;
		Entity(entt::entity entityHandle, World * context);
		~Entity()=default;
		virtual void UpdateData(float DeltaTime)override;
		virtual void Destroy()override;
		virtual void OnDestroy()override;


		template<typename T>
		bool HasComponent()const { return m_Context->m_Registry.any_of<T>(m_Handle); }
		template<typename T, typename...Args>
		T* AddComponent(Args&&...arg)
		{
			if (HasComponent<T>()) { LOG_CORE_WARN("Entity already has component!"); return nullptr; }
			return &m_Context->m_Registry.emplace<T>(m_Handle, std::forward<Args>(arg)...);
		}
		template<typename T>
		void RemoveComponent()
		{
			if (!HasComponent<T>()) { LOG_CORE_WARN("Entity does not have component,need not to remove"); return; }
			m_Context->m_Registry.get<T>(m_Handle);
		}
		template<typename T>
		T* GetComponent()const
		{
			if (!HasComponent<T>()) { LOG_CORE_WARN("Entity does not have component!"); return nullptr; }
			return &m_Context->m_Registry.get<T>(m_Handle);
		}

		operator bool()const { return m_Handle != entt::null; }
		operator entt::entity()const { return m_Handle; }
		bool operator==(const Entity& other) { return m_Handle == other.m_Handle; }
		bool operator!=(const Entity& other) { return !(*this == other); }
	private:
		entt::entity m_Handle{ entt::null };
		World* m_Context = nullptr;
	};

	class DM_API ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() = default;
	protected:
		virtual void OnCreate() {};
		virtual void OnUpdate(float deltaTime) {};
		virtual void OnDestroy() {};
	private:
		Entity m_Entity;
	};
}
