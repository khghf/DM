#include<Editor/Core/EditorContext.h>
#include<Engine.h>
#include<DM.h>
#include<Editor/Commands/ICommand.h>
#include<Core/AssetManagent/AssetObject/AssetObjectSetter.h>
#include<Framework/Base/WorldSerializer.h>
namespace DM
{
	bool EditorContext::SaveCurrentWorldIfModified()
	{
		// 以活动世界为准(与命令谓词/删除逻辑同源)，避免 m_EditorWorld 与活动世界不一致
		SPtr<World> world = GetActiveWorld();
		if (!world || !world->IsDirty()) return true;

		// 保存逻辑统一收敛到 CmdSaveWorld 命令(路径解析/序列化/落盘/状态更新均在其中实现)
		CmdSaveWorld cmd;
		cmd.Execute();

		// 保存成功会清除 dirty 标记；用户取消另存对话框则仍为 dirty → 返回 false 中止切换
		return !world->IsDirty();
	}

	bool EditorContext::ChangeEditorWorld(const SPtr<World>& world)
	{
		if (!world) return false;
		if (IsPlaying()) return false;                     // 播放模式下不允许切换编辑器世界
		if (!SaveCurrentWorldIfModified()) return false;   // 用户取消另存时中止切换

		m_EditorWorld = world;
		// SetActiveWorld 内部处理：清空实体选择(防止旧 World 释放后悬垂) + 通知面板刷新
		SetActiveWorld(world);
		Engine::Get()->GetGameInst().SetCurWorld(world);

		LOG_CORE_INFO("Editor world changed: {}", world->m_Name);
		return true;
	}

	bool EditorContext::EnterPlayMode()
	{
		if (IsPlaying() || !m_EditorWorld) return false;

		// 深拷贝编辑器世界作为播放世界(entt::registry 不可拷贝，用序列化快照重建)
		m_PlayWorld = CreateSPtr<World>();
		m_PlayWorld->m_Name = m_EditorWorld->m_Name;
		m_PlayWorld->m_path = m_EditorWorld->m_path;
		m_PlayWorld->bHasSavedToLocal = m_EditorWorld->bHasSavedToLocal;
		// 播放世界继承资产身份，保证保存/反查路径一致
		AssetObjectSetter::SetAssetId(m_PlayWorld.get(), m_EditorWorld->GetAssetId());

		const auto snapshots = WorldSerializer::SerializeWorld(m_EditorWorld.get());
		WorldSerializer::DeserializeWorld(m_PlayWorld.get(), snapshots);

		// SetActiveWorld 内部赋值 m_ActiveWorld + 清空选择 + 通知面板刷新
		SetActiveWorld(m_PlayWorld);
		SetPlayMode(EditorPlayMode::Play);
		Engine::Get()->GetGameInst().SetCurWorld(m_PlayWorld);

		LOG_CORE_INFO("Enter play mode");
		return true;
	}

	void EditorContext::ExitPlayMode()
	{
		if (!IsPlaying()) return;

		// 丢弃播放世界，回到编辑器世界(SetActiveWorld 内部赋值 m_ActiveWorld + 清空选择 + 通知刷新)
		m_PlayWorld.reset();
		SetActiveWorld(m_EditorWorld);
		SetPlayMode(EditorPlayMode::Edit);
		Engine::Get()->GetGameInst().SetCurWorld(m_EditorWorld);

		LOG_CORE_INFO("Exit play mode");
	}
}
