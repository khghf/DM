#pragma once
#include"DM.h"
#include<Framework/Base/Entity.h>
#include<Framework/Base/World.h>
#include<Core/AssetManagent/AsetPack/AssetPack.h>
#include"Editor/Services/SelectionManager.h"
#include"Editor/Core/Service/ServiceRegistry.h"
#include<atomic>
#include<memory>

namespace DM
{
	enum class EditorPlayMode : uint8_t
	{
		Edit = 0,
		Play
	};

	/// <summary>
	/// 编辑器全局上下文：只负责世界管理(编辑器世界/播放世界/活动世界/切换/播放模式)，
	/// 以及为面板提供变更通知与服务注册表(组合根)。
	/// 选中状态(实体/文件/目录)统一由 SelectionManager 服务管理，不在本类持有。
	/// </summary>
	class EditorContext
	{
	public:
		static EditorContext* Get()
		{
			static EditorContext inst;
			return &inst;
		}

		// ==================== 世界状态 ====================

		/// <summary>活动世界(Edit→编辑器世界 / Play→播放世界)，命令谓词/删除逻辑同源</summary>
		SPtr<World> GetActiveWorld() const { return m_ActiveWorld; }
		void SetActiveWorld(const SPtr<World>& world)
		{
			if (m_ActiveWorld == world) return;   // 未变化：跳过(避免切换前 SaveCurrentWorldIfModified 失效)
			m_ActiveWorld = world;
			// 世界切换：清空实体选择，防止旧 World 释放后 Entity 悬垂(旧 Entity 持有 World* 裸指针)
			GetService<SelectionManager>()->ClearAll();
			NotifyChanged(); // 世界切换通知：面板据此重建缓存(层级/属性等)
		}

		/// <summary>编辑器世界(编辑模式下的场景数据源)</summary>
		SPtr<World> GetEditorWorld() const { return m_EditorWorld; }
		/// <summary>播放世界(播放模式的场景数据源，进入播放时由编辑器世界深拷贝而来)</summary>
		SPtr<World> GetPlayWorld() const { return m_PlayWorld; }

		/// <summary>
		/// 切换编辑器世界：播放模式下拒绝；切换前自动保存当前世界的未保存修改(用户取消则中止)。
		/// 成功后更新活动世界并同步引擎。返回是否完成切换。
		/// </summary>
		bool ChangeEditorWorld(const SPtr<World>& world);
		/// <summary>进入播放模式：深拷贝编辑器世界作为播放世界并激活(失败返回 false)</summary>
		bool EnterPlayMode();
		/// <summary>退出播放模式：丢弃播放世界，回到编辑器世界</summary>
		void ExitPlayMode();

		// ==================== 变更通知 ====================

		/// <summary>标记当前世界有未保存修改(组件/实体变更后调用)，供切换世界前保存判定</summary>
		void NotifyWorldModified()
		{
			if (m_ActiveWorld) m_ActiveWorld->MarkDirty();
		}

		EditorPlayMode GetPlayMode() const { return m_PlayMode; }
		bool IsPlaying() const { return m_PlayMode == EditorPlayMode::Play; }
		void SetPlayMode(EditorPlayMode mode) { m_PlayMode = mode; }

		uint64_t GetChangeCount() const { return m_ChangeCount.load(std::memory_order_relaxed); }
		void NotifyChanged() { m_ChangeCount.fetch_add(1, std::memory_order_relaxed); }

		/// <summary>服务注册表(组合根)：服务在此统一装配(创建/Init/Shutdown)</summary>
		ServiceRegistry& GetServiceRegistry() { return *m_ServiceRegistry; }

		/// <summary>便捷查询服务：EditorContext::GetService&lt;T&gt;()</summary>
		template<typename T>
		static T* GetService() { return Get()->m_ServiceRegistry->Get<T>(); }

	private:
		EditorContext() = default;

		/// <summary>
		/// 切换世界前的保存保护：当前世界有未保存修改时先保存(有路径静默保存；从未落盘弹另存框)。
		/// 保存成功(清除 dirty)返回 true；用户取消另存返回 false。保存实现统一委托 CmdSaveWorld 命令。
		/// </summary>
		bool SaveCurrentWorldIfModified();

	private:
		SPtr<World> m_EditorWorld;   // 编辑器世界(场景编辑数据源)
		SPtr<World> m_PlayWorld;     // 播放世界(播放模式的运行时副本)
		SPtr<World> m_ActiveWorld;   // 活动世界(Edit→编辑器世界 / Play→播放世界)
		EditorPlayMode m_PlayMode = EditorPlayMode::Edit;
		std::atomic<uint64_t> m_ChangeCount{ 0 };
		std::unique_ptr<ServiceRegistry> m_ServiceRegistry = std::make_unique<ServiceRegistry>();
	};
}
