#pragma once
#include <Core/Core.h>
#include <Core/Reflection/Mirror/include/mirror.h>
#include <string>
#include <vector>
#include <cstdint>

namespace DM
{
	class World;

	/// <summary>
	/// 单个组件的持久化数据：类型名(跨进程稳定，TypeName<T> 编译期字符串) + 序列化字节。
	/// 组件对象先由 BinarySerializer 特化序列化进 m_Data，再随 WorldPack 落盘。
	/// </summary>
	struct ComponentData
	{
		std::string m_TypeName;
		std::vector<uint8_t> m_Data;
	};

	/// <summary>
	/// 单个实体的持久化快照：实体ID + version + 组件列表。
	/// </summary>
	struct EntitySnapshot
	{
		uint32_t m_EntityId = 0;   // entt 实体索引
		uint8_t  m_Version = 0;    // entt version 段(实体销毁后重建版本递增)
		std::vector<ComponentData> m_Components;

		// 有效快照判定：SerializeEntity 对无效实体返回全零默认快照
		// (m_EntityId==0 且无组件)。合法实体(即便无组件)至少 m_EntityId 非 0；
		// 仅索引0+version0 的裸实体与无效快照无法区分，属极端罕见，可接受。
		bool IsValid() const { return m_EntityId != 0 || !m_Components.empty(); }
	};

	/// <summary>
	/// 世界序列化器：World <-> 实体快照数组。
	/// 保存：遍历 registry 收集每个实体的组件 -> vector&lt;EntitySnapshot&gt;；
	/// 加载：按快照精确重建实体ID/version 并逐组件反序列化。
	/// 快照数组本身是普通可反射结构，由 WorldPack::m_Entities 随包落盘。
	/// </summary>
	class DM_API WorldSerializer
	{
	public:
		/// <summary>收集 World 全部实体为快照数组(不修改 World)</summary>
		static std::vector<EntitySnapshot> SerializeWorld(const World* world);

		/// <summary>用快照数组重建 World 的实体(先清空目标 registry)</summary>
		static bool DeserializeWorld(World* world, const std::vector<EntitySnapshot>& snapshots);

		/// <summary>序列化单个实体为快照(不修改 World)，entityId 为 entt 实体完整编码</summary>
		static EntitySnapshot SerializeEntity(const World* world, uint32_t entityId);

		/// <summary>
		/// 用单实体快照恢复实体：
		/// 目标实体仍存在则清空其全部组件后恢复；不存在则按快照索引+version 精确重建；
		/// 索引已被新实体占用时创建全新实体兜底。成功时经 outEntityId 带出实际实体完整编码。
		/// </summary>
		static bool DeserializeEntity(World* world, const EntitySnapshot& snapshot, uint32_t* outEntityId = nullptr);
	};

	// 反射注册：标准布局 struct，供 WorldPack::m_Entities(vector<EntitySnapshot>) 直接序列化
	REGISTER_TYPE(ComponentData);
	REGISTER_MEMBER(ComponentData, m_TypeName);
	REGISTER_MEMBER(ComponentData, m_Data);
	REGISTER_TYPE(EntitySnapshot);
	REGISTER_MEMBER(EntitySnapshot, m_EntityId);
	REGISTER_MEMBER(EntitySnapshot, m_Version);
	REGISTER_MEMBER(EntitySnapshot, m_Components);
}
