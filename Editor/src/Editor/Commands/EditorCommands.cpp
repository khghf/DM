#include<Editor/Commands/ICommand.h>
#include<Editor/Core/EditorContext.h>
#include<Editor/Services/SelectionManager.h>
#include<Foundation/Util/PlatformUtils.h>
#include<Framework/Base/World.h>
#include<Framework/Base/WorldSerializer.h>
#include<Core/AssetManagent/AssetMgr.h>
#include<Core/AssetManagent/AssetMetaDatabase.h>
#include<Core/AssetManagent/AsetPack/WorldPack.h>
#include<Core/AssetManagent/AssetUtil.h>
#include<Core/AssetManagent/AssetObject/AssetObjectSetter.h>
#include<Foundation/FileSystem.h>
#include<algorithm>
namespace DM
{
	namespace
	{
		/// <summary>
		/// 打开场景公共路径：加载 .world 资产并切换到新世界(切换前自动保存保护)。
		/// Execute/Undo/Redo 复用，避免三处重复逻辑。
		/// </summary>
		bool OpenWorldImpl(const std::string& path)
		{
			if (path.empty() || std::filesystem::path(path).extension() != ".world") return false;

			SPtr<World> world = AssetMgr::LoadAsset<World>(path);
			if (!world)
			{
				LOG_CORE_ERROR("Open scene failed: {}", path);
				return false;
			}
			// 世界切换(含未保存修改保护)统一由 EditorContext 管理
			return EditorContext::Get()->ChangeEditorWorld(world);
		}
	}

	// ============================ CmdOpenWorld ============================
	bool CmdOpenWorld::Execute()
	{
		std::string path = FileDialog::OpenFile("(*.world)\0*.world\0");
		if (path.empty()) return false;

		// 记录切换前场景的 AssetID（AssetObject 提供），Undo 时经资产库反查源路径回到原场景
		if (SPtr<World> cur = EditorContext::Get()->GetActiveWorld())
		{
			m_PrevAssetId = cur->GetAssetId();
		}

		m_OpenedPath = path; // Redo 时直接重放本次打开的场景，避免再次弹文件对话框
		return OpenWorldImpl(path);
	}

	void CmdOpenWorld::Undo()
	{
		if (!m_PrevAssetId.IsValid()) return;

		std::string prevPath = AssetMetaDatabase::Get()->GetSourceFilePathByGuid(m_PrevAssetId);
		if (!prevPath.empty()) OpenWorldImpl(prevPath);
	}

	void CmdOpenWorld::Redo()
	{
		if (m_OpenedPath.empty()) return;

		OpenWorldImpl(m_OpenedPath);
	}

	// ============================ CmdSaveWorld ============================
	bool CmdSaveWorld::Execute()
	{
		// 与命令谓词/删除逻辑同源：活动世界统一经 EditorContext 获取(Editor::GetActiveWorld 亦委托它)
		SPtr<World> world = EditorContext::Get()->GetActiveWorld();
		if (!world) return false;

		// 版本标识：确认保存逻辑确实在命令内执行(旧代码走 Editor::SaveScene 无此前缀日志)
		LOG_CORE_INFO("[CmdSaveWorld] saving world '{}' (ptr={})", world->m_Name, static_cast<void*>(world.get()));

		// 有资产记录时直接取源路径落盘，不弹保存对话框
		std::string path = AssetMetaDatabase::Get()->GetSourceFilePathByGuid(world->GetAssetId());
		if (path.empty())
		{
			// 未落盘过(资产库无记录)：回退到另存对话框
			path = FileDialog::SaveFile();
			if (path.empty()) return false;
		}

		// ---- 保存核心逻辑(统一收敛到命令：复用已有资产包保持 GUID 身份，再序列化落盘) ----
		WorldPack pack{};
		if (std::filesystem::exists(path))
		{
			try
			{
				AssetUtil::Deserialize(&pack, path);
			}
			catch (...)
			{
				// 旧格式/损坏：重建包(重新保存即可)
				LOG_CORE_WARN("[CmdSaveWorld]failed to read existing pack '{}', will create a new one", path);
			}
		}

		if (!pack.GetGUID().IsValid())
		{
			AssetPackSetter::SetGuid(&pack, AssetID::GenNewID());
		}

		AssetMetaInfo metaInfo = pack.GetMeta();
		metaInfo.m_Type = EAssetType::World;
		metaInfo.m_SourceFilePath = path;
		metaInfo.m_SourceFileLastModifyTime = AssetUtil::TimeNow();
		AssetPackSetter::SetMetaInfo(&pack, metaInfo);

		pack.m_WorldName = world->m_Name;
		pack.m_Entities = WorldSerializer::SerializeWorld(world.get());

		// 过滤"存在但无组件"的幽灵实体(历史上残留的无意义裸实体)：它们不携带任何数据，
		// 序列化只会让加载后重新出现无组件实体(层级面板显示为 Entity，访问组件报错)。
		// 注意：不能连实体ID一起保留——空组件快照会在加载时 create 出裸实体
		const auto ghostCount = std::count_if(pack.m_Entities.begin(), pack.m_Entities.end(),
			[](const EntitySnapshot& snap) { return snap.m_Components.empty(); });
		if (ghostCount > 0)
		{
			LOG_CORE_WARN("[CmdSaveWorld] skip {} ghost entit{} with no components", ghostCount, ghostCount > 1 ? "ies" : "y");
			pack.m_Entities.erase(std::remove_if(pack.m_Entities.begin(), pack.m_Entities.end(),
				[](const EntitySnapshot& snap) { return snap.m_Components.empty(); }), pack.m_Entities.end());
		}

		// 落盘 + 注册数据库(保证资产管线可再次加载)
		const std::string finalPath = AssetUtil::SerializePack(&pack, path);

		// 更新 World 状态
		world->m_path = finalPath;
		world->bHasSavedToLocal = true;
		world->m_Name = FileSystem::GetFileName(finalPath);
		// 资产对象持有包内 GUID，后续可通过资产库反查源文件路径
		AssetObjectSetter::SetAssetId(world.get(), pack.GetGUID());
		world->ClearDirty();   // 已落盘，清除未保存标记

		LOG_CORE_INFO("[CmdSaveWorld] World saved: {} ({} entities)", finalPath, pack.m_Entities.size());
		return false; // 保存是落盘动作，不进入撤销历史
	}

	void CmdSaveWorld::Undo() {}

	// ============================ CmdPlayMode ============================
	bool CmdPlayMode::Execute()
	{
		EditorContext::Get()->EnterPlayMode();
		return false; // 播放切换是动作，不进入撤销历史
	}

	void CmdPlayMode::Undo() {}

	// ============================ CmdStopPlayMode ============================
	bool CmdStopPlayMode::Execute()
	{
		EditorContext::Get()->ExitPlayMode();
		return false; // 播放切换是动作，不进入撤销历史
	}

	void CmdStopPlayMode::Undo() {}

	// ============================ CmdDeleteEntity ============================
	bool CmdDeleteEntity::Execute()
	{
		EditorContext* ctx = EditorContext::Get();
		SelectionManager* sel = EditorContext::GetService<SelectionManager>();
		if (!sel->HasSelectedEntity()) return false;

		World* world = ctx->GetActiveWorld().get();
		if (!world) return false;

		// 收集全部选中实体(拷贝集合，执行中集合不再变化)，多选批量删除
		const std::vector<Entity> entities = sel->GetSelectedEntities();
		m_EntitySnapshots.clear();
		m_RestoredEntityIds.clear();

		for (const Entity& entity : entities)
		{
			if (!entity) continue;
			// 删除前只保存该实体快照，Undo 时按序恢复
			// 注意：必须先显式转 entt::entity 再取完整编码——直接 static_cast<uint32_t>(entity)
			// 会因重载决议选中 operator bool()(返回 0/1)，导致快照记录到错误的实体
			EntitySnapshot snap = WorldSerializer::SerializeEntity(world, static_cast<uint32_t>(static_cast<entt::entity>(entity)));
			// 快照有效才记录(供 Undo 恢复)；无效(如索引0的无组件裸实体/其它世界残留句柄)不记录——
			// 但删除动作不再受快照影响：只要实体在当前世界 registry 中有效就销毁，杜绝"删不掉"的幽灵实体
			if (snap.IsValid())
			{
				m_EntitySnapshots.push_back(std::move(snap));
			}
			else
			{
				LOG_CORE_WARN("[CmdDeleteEntity] skip invalid entity snapshot id={}", (uint32_t)(entt::entity)entity);
			}
			// 防御选中集合残留其它世界的句柄：仅销毁当前世界 registry 中有效的实体
			if (world->GetEnttRegistry().valid(static_cast<entt::entity>(entity)))
			{
				world->DestroyEntity(entity);
			}
		}

		sel->ClearAll();
		ctx->NotifyChanged();
		// 活实体数 = free_list()（entity storage 是 swap_only 软删设计，size() 含 free 区）
		const auto* entityStorage = world->GetEnttRegistry().storage<entt::entity>();
		LOG_CORE_INFO("[CmdDeleteEntity] Delete {} entities, snapshots saved: {} (world ptr={}, remaining={})",
			entities.size(), m_EntitySnapshots.size(), static_cast<void*>(world),
			entityStorage ? static_cast<uint32_t>(entityStorage->free_list()) : 0u);
		return !m_EntitySnapshots.empty();
	}

	void CmdDeleteEntity::Undo()
	{
		World* world = EditorContext::Get()->GetActiveWorld().get();
		if (!world) return;

		SelectionManager* sel = EditorContext::GetService<SelectionManager>();
		sel->ClearAll();
		m_RestoredEntityIds.clear();
		m_RestoredEntityIds.reserve(m_EntitySnapshots.size());

		// 逆序恢复(与删除顺序相反，还原删除前的排列)；恢复后全部选中，还原现场
		for (auto it = m_EntitySnapshots.rbegin(); it != m_EntitySnapshots.rend(); ++it)
		{
			uint32_t restoredId = 0;
			if (WorldSerializer::DeserializeEntity(world, *it, &restoredId))
			{
				m_RestoredEntityIds.push_back(restoredId); // 记录实际恢复身份，Redo 据此删除
				sel->AddEntity(Entity(entt::entity(restoredId), world));
			}
		}
		EditorContext::Get()->NotifyChanged(); // 恢复后刷新面板
	}

	void CmdDeleteEntity::Redo()
	{
		World* world = EditorContext::Get()->GetActiveWorld().get();
		if (!world) return;

		// 重放删除：删除 Undo 时恢复出的全部实体，不重新序列化快照
		SelectionManager* sel = EditorContext::GetService<SelectionManager>();
		sel->ClearAll();
		for (const uint32_t id : m_RestoredEntityIds)
		{
			const entt::entity entity = entt::entity(id);
			if (world->GetEnttRegistry().valid(entity))
			{
				world->DestroyEntity(Entity(entity, world));
			}
		}
		m_RestoredEntityIds.clear();
		EditorContext::Get()->NotifyChanged(); // 删除后刷新面板
	}

	// ============================ CmdDuplicateEntity ============================
	bool CmdDuplicateEntity::Execute()
	{
		EditorContext* ctx = EditorContext::Get();
		SelectionManager* sel = EditorContext::GetService<SelectionManager>();
		if (!sel->HasSelectedEntity()) return false;

		World* world = ctx->GetActiveWorld().get();
		if (!world) return false;

		// 收集全部选中实体(拷贝集合)，多选批量复制
		const std::vector<Entity> entities = sel->GetSelectedEntities();
		m_Items.clear();

		for (const Entity& src : entities)
		{
			if (!src) continue;
			// 只记录源实体快照，复制 = 快照改写到新身份后恢复
			// 与删除命令相同：显式经 entt::entity 转换，避免 operator bool() 误取 0/1
			DuplicateItem item;
			item.Source = WorldSerializer::SerializeEntity(
				world, static_cast<uint32_t>(static_cast<entt::entity>(src)));

			Entity dup = world->CreateEntity();
			item.NewEntityId = static_cast<uint32_t>(static_cast<entt::entity>(dup));

			EntitySnapshot dupSnapshot = item.Source;
			dupSnapshot.m_EntityId = item.NewEntityId;
			dupSnapshot.m_Version = static_cast<uint8_t>(entt::to_version(entt::entity(item.NewEntityId)));

			uint32_t restoredId = 0;
			if (!WorldSerializer::DeserializeEntity(world, dupSnapshot, &restoredId))
			{
				// 失败则清理新实体，避免残留
				world->DestroyEntity(dup);
				continue;
			}
			item.NewEntityId = restoredId; // 记录实际身份，Undo/Redo 据此操作
			m_Items.push_back(std::move(item));
		}

		// 选中复制出的全部新实体
		sel->ClearAll();
		for (const DuplicateItem& item : m_Items)
		{
			sel->AddEntity(Entity(entt::entity(item.NewEntityId), world));
		}
		ctx->NotifyChanged(); // 实体增删通知：面板据此重建缓存
		return !m_Items.empty();
	}

	void CmdDuplicateEntity::Undo()
	{
		World* world = EditorContext::Get()->GetActiveWorld().get();
		if (!world) return;

		// 只销毁复制出的新实体，其余世界不受影响
		SelectionManager* sel = EditorContext::GetService<SelectionManager>();
		sel->ClearAll();
		for (const DuplicateItem& item : m_Items)
		{
			const entt::entity dup = entt::entity(item.NewEntityId);
			if (world->GetEnttRegistry().valid(dup))
			{
				world->DestroyEntity(Entity(dup, world));
			}
		}
		EditorContext::Get()->NotifyChanged(); // 恢复后刷新面板
	}

	void CmdDuplicateEntity::Redo()
	{
		World* world = EditorContext::Get()->GetActiveWorld().get();
		if (!world) return;

		// 重放复制：用记录的快照改写到新身份后重建副本(不重新序列化源实体)
		SelectionManager* sel = EditorContext::GetService<SelectionManager>();
		sel->ClearAll();
		for (const DuplicateItem& item : m_Items)
		{
			EntitySnapshot dupSnapshot = item.Source;
			dupSnapshot.m_EntityId = item.NewEntityId;
			dupSnapshot.m_Version = static_cast<uint8_t>(entt::to_version(entt::entity(item.NewEntityId)));

			uint32_t restoredId = 0;
			if (WorldSerializer::DeserializeEntity(world, dupSnapshot, &restoredId))
			{
				sel->AddEntity(Entity(entt::entity(restoredId), world));
			}
		}
		EditorContext::Get()->NotifyChanged(); // 实体增删通知：面板据此重建缓存
	}
}
