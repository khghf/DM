#pragma once
#include <map>
#include <string>
#include <Core/Reflection/Mirror/include/mirror.h>
#include <entt/entt.hpp>

namespace DM
{
	/// <summary>
	/// entt 侧类型擦除适配器：序列化/反序列化由 Mirror TypeInfo 运行期分派，
	/// 这里只保留 entt 交互(必须编译期类型)的最小三件套。
	/// </summary>
	template <typename T>
	struct ComponentCodec
	{
		static bool         Has(entt::registry const& registry, entt::entity e) { return registry.all_of<T>(e); }
		static const void*  Get(entt::registry const& registry, entt::entity e) { return &registry.get<T>(e); }
		static void*        EmplaceDefault(entt::registry& registry, entt::entity e) { return &registry.emplace<T>(e); }
		static void         Remove(entt::registry& registry, entt::entity e) { registry.remove<T>(e); }
	};

	/// <summary>
	/// 运行期组件条目：TypeInfo(含 BinarySerializer/BinaryDeserializer 函数指针) + entt 适配器。
	/// 由 AutoRegisterComponent 在 main 前注册。
	/// </summary>
	struct ComponentEntry
	{
		const mirror::TypeInfo* TypeInfo = nullptr;
		bool         (*Has)(entt::registry const&, entt::entity) = nullptr;
		const void*  (*Get)(entt::registry const&, entt::entity) = nullptr;
		void*        (*EmplaceDefault)(entt::registry&, entt::entity) = nullptr;
		void         (*Remove)(entt::registry&, entt::entity) = nullptr;
	};

	/// <summary>
	/// 全局组件注册表：TypeName(编译期稳定串) -> 组件条目。
	/// </summary>
	inline std::map<std::string, ComponentEntry>& GetComponentTable()
	{
		static std::map<std::string, ComponentEntry> table;
		return table;
	}

	/// <summary>
	/// 组件反射成员注册 hook：默认空实现(无反射成员)。有需要序列化成员的组件，
	/// 须在组件头文件提供显式特化(见 REGISTER_COMPONENT_MEMBERS 宏)。
	/// 背景：REGISTER_MEMBER 生成的 inline static 注册器(仅构造副作用、无外部引用)在
	/// Editor/Release 配置(链接器 /OPT:REF 默认开启)下会被丢弃，导致 TypeInfo.Members
	/// 为空、组件序列化不出任何字段(加载后组件全是默认值，Tag 空 => 层级面板显示 "Entity")。
	/// 因此由 AutoRegisterComponent<T>::Register() 运行时显式调用，链接器必然保留注册代码。
	/// </summary>
	template <typename T>
	inline void RegisterComponentMembers() {}

	/// <summary>
	/// 注册单个组件成员(等价于 REGISTER_MEMBER 的运行时行为，供组件头宏调用)。
	/// </summary>
	template <typename Class, typename Field>
	inline void RegisterComponentMember(std::string_view fieldName, uint32_t offset, uint32_t size, uint32_t align)
	{
		mirror::AutoRegisterMember reg{
			static_cast<Class*>(nullptr),
			static_cast<Field*>(nullptr),
			mirror::VariableId::Create<Field>(),
			fieldName, offset, size, align,
			mirror::DefaultMemberProperties };
		(void)reg;
	}

	/// <summary>
	/// 组件注册器：构造时把 T 的 TypeInfo + entt 适配器写入全局表。
	/// 由 REGISTER_COMPONENT 宏生成为 inline static 变量，main 前恰好注册一次。
	/// 注意：inline static 变量仅构造有副作用、无外部引用，链接器 /OPT:REF(Editor/Release
	/// 配置默认开启)可能丢弃，导致组件表为空、世界序列化丢失全部组件。因此把注册逻辑
	/// 抽成静态函数 Register()，序列化路径(WorldSerializer)会显式调用一次做兜底。
	/// Register() 同时显式补齐反射成员注册(REGISTER_MEMBER 的注册器同样会被丢弃)。
	/// </summary>
	template <typename T>
	struct AutoRegisterComponent
	{
		AutoRegisterComponent() { Register(); }

		static void Register()
		{
			GetComponentTable().emplace(
				std::string(mirror::TypeName<T>()),
				ComponentEntry{ &mirror::RegisterType<T>(),
								&ComponentCodec<T>::Has,
								&ComponentCodec<T>::Get,
								&ComponentCodec<T>::EmplaceDefault,
								&ComponentCodec<T>::Remove });
			RegisterComponentMembers<T>();
		}
	};
}

#define DM_CONCAT_(a, b) a##b
#define DM_CONCAT(a, b)  DM_CONCAT_(a, b)

/// <summary>
/// 声明式组件注册：一行完成 Mirror 反射注册 + entt 适配器注册。
/// 序列化/反序列化由 Mirror TypeInfo 自动绑定默认序列化函数(遍历 REGISTER_MEMBER 注册的成员)，
/// 类型枚举由 GetComponentTable() 自动收集，WorldSerializer 无需再感知具体组件。
/// </summary>
#define REGISTER_COMPONENT(T)                                              \
	REGISTER_TYPE(T);                                                      \
	inline static DM::AutoRegisterComponent<T> DM_CONCAT(RegisterComponent_, __COUNTER__) {}

// 单个成员注册语句(须在类定义完整后使用)：构造 AutoRegisterMember 绑定成员到 TypeInfo。
// 与 REGISTER_MEMBER 等价，但由 RegisterComponentMembers<T>() 特化在运行时显式调用，
// 不会被链接器 /OPT:REF 丢弃。
#define REGISTER_COMPONENT_MEMBER(TYPE, MEMBER)                                 \
	DM::RegisterComponentMember<TYPE, decltype(TYPE::MEMBER)>(#MEMBER, offsetof(TYPE, MEMBER), sizeof(decltype(TYPE::MEMBER)), alignof(decltype(TYPE::MEMBER)))

// 组件反射成员注册声明：为 TYPE 提供 RegisterComponentMembers<TYPE>() 显式特化。
// 用法(放在 REGISTER_COMPONENT(TYPE) 之后)：
//   REGISTER_COMPONENT_MEMBERS(TYPE,
//       REGISTER_COMPONENT_MEMBER(TYPE, MemberA);
//       REGISTER_COMPONENT_MEMBER(TYPE, MemberB);
//   );
#define REGISTER_COMPONENT_MEMBERS(TYPE, ...)                                     \
	template <> inline void DM::RegisterComponentMembers<TYPE>() { __VA_ARGS__ }
