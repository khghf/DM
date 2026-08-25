#include <DMPCH.h>
#include "Editor/UI/Panels/SceneHierarchyPanel.h"
#include <Editor/UI/EditorUI.h>
#include <Framework/Base/Entity.h>
#include <Framework/Component/Component.h>
#include <Editor/Core/EditorContext.h>
#include <Editor/Services/SelectionManager.h>
#include <Editor/Commands/ICommand.h>
#include <Editor/Commands/CommandInvoker.h>

namespace DM
{
	void SceneHierarchyPanel::Render()
	{
		EditorUI::Begin("Scene Hierarchy");
		{
			// 搜索过滤
			EditorUI::InputText("Filter", m_Filter, sizeof(m_Filter));
			EditorUI::Separator();

			// 感知编辑器上下文变更(实体增删/标签修改等)，自动重建缓存
			uint64_t changeCount = EditorContext::Get()->GetChangeCount();
			if (changeCount != m_LastChangeCount)
			{
				m_LastChangeCount = changeCount;
				m_CacheDirty = true;
			}

			// 重建缓存
			RebuildCacheIfNeeded();

			// 获取过滤后的列表
			const auto& list = GetFilteredEntities();

			// 虚拟列表：只渲染可见的
			ImGuiListClipper clipper;
			clipper.Begin(static_cast<int>(list.size()));

			while (clipper.Step())
			{
				for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
				{
					DrawEntityNode(list[i]);
				}
			}
			clipper.End();

			// 点击空白区域清空实体选中(不影响文件/目录选中；命中列表项时不触发)
			if (EditorUI::IsWindowHovered() && EditorUI::IsMouseClicked(ImGuiMouseButton_Left) && !EditorUI::IsAnyItemHovered())
			{
				EditorContext::GetService<SelectionManager>()->ClearEntities();
			}

			// 右键菜单：创建实体
			if (EditorUI::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
			{
				static int createCount = 10;
				EditorUI::DragInt("Count", &createCount, 1.f, 1, 100);

				if (EditorUI::MenuItem("Create Entity"))
				{
					const SPtr<World>& world = EditorContext::Get()->GetActiveWorld();
					for (int i = 0; i < createCount && world; i++)
					{
						world->CreateEntity();
					}
					m_CacheDirty = true;
					EditorContext::Get()->NotifyChanged();
				}
				EditorUI::EndPopup();
			}
		}
		EditorUI::End();
	}

	void SceneHierarchyPanel::RebuildCacheIfNeeded()
	{
		if (!m_CacheDirty) return;

		m_CachedEntities.clear();

		const SPtr<World>& world = EditorContext::Get()->GetActiveWorld();
		if (!world) return;

		const auto& registry = world->GetEnttRegistry();  
		for (auto entity : registry.view<entt::entity>())
		{
			if (!registry.valid(entity)) continue;

			Entity en{ entity, world.get() };
			auto* tag = en.GetComponent<TagComponent>();

			CachedEntity cached;
			cached.Handle = entity;
			cached.Tag = tag ? tag->Tag : "Entity";

			m_CachedEntities.push_back(std::move(cached));
		}

		m_CacheDirty = false;
	}


	const std::vector<SceneHierarchyPanel::CachedEntity>&
		SceneHierarchyPanel::GetFilteredEntities()
	{
		if (m_Filter[0] == '\0')
		{
			return m_CachedEntities;
		}

		m_FilteredEntities.clear();
		for (const auto& entity : m_CachedEntities)
		{
			if (entity.Tag.find(m_Filter) != std::string::npos)
			{
				m_FilteredEntities.push_back(entity);
			}
		}
		return m_FilteredEntities;
	}

	
	void SceneHierarchyPanel::DrawEntityNode(const CachedEntity& cached)
	{
		ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_FramePadding |
			ImGuiTreeNodeFlags_Leaf;  // 没有子节点

		const SPtr<World>& world = EditorContext::Get()->GetActiveWorld();
		if (!world) return;

		Entity entity{ cached.Handle, world.get() };
		SelectionManager* sel = EditorContext::GetService<SelectionManager>();

		// 高亮所有选中的实体(多选集合统一存于 SelectionManager)
		if (sel->ContainsEntity(entity))
		{
			flag |= ImGuiTreeNodeFlags_Selected;
		}

		bool opened = EditorUI::TreeNodeEx(
			(void*)(uint64_t)cached.Handle,
			flag,
			"%s", cached.Tag.c_str());

		if (EditorUI::BeginPopupContextItem())
		{
			if (EditorUI::MenuItem("Delete Entity"))
			{
				// 与 Delete 快捷键走同一条命令链：右键项不在选中集合时先单选它，
				// 再 Invoke 删除——命令读取整个多选集合，实现多选批量删除
				if (!sel->ContainsEntity(entity))
				{
					sel->SetSelectedEntity(entity);
				}
				CommandInvoker::Invoke<CmdDeleteEntity>();
			}
			EditorUI::EndPopup();
		}

		if (EditorUI::IsItemClicked())
		{
			// 多选交互：普通点击=单选；Ctrl+点击=切换；Shift+点击=按锚点范围选择
			const bool ctrl = EditorUI::GetIO().KeyCtrl;
			const bool shift = EditorUI::GetIO().KeyShift;

			if (ctrl)
			{
				sel->ToggleEntity(entity);
			}
			else if (shift)
			{
				// 以当前过滤后的显示顺序作为范围选择的顺序基准
				const auto& filtered = GetFilteredEntities();
				std::vector<Entity> ordered;
				ordered.reserve(filtered.size());
				for (const auto& c : filtered)
				{
					ordered.push_back(Entity{ c.Handle, world.get() });
				}
				sel->SelectEntityRange(ordered, entity);
			}
			else
			{
				sel->SetSelectedEntity(entity);
			}
		}

		if (opened)
		{
			EditorUI::TreePop();
		}
	}
}
