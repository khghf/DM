#include "Editor/Core/EditorApplication.h"
#include <Engine.h>
#include<DM.h>
#include<Core/AssetManagent/AssetMgr.h>
#include"Editor/UI/Panels/SceneHierarchyPanel.h"
#include"Editor/UI/Panels/PropertiesPanel.h"
#include"Editor/UI/Panels/ContentBrowserPanel.h"
#include"Editor/UI/Panels/ImportAssetDialogPanel.h"
#include"Editor/UI/Panels/StylePanel.h"
#include<Editor/UI/Style/EditorStyle.h>
#include<Editor/Core/EditorContext.h>
#include<Editor/Services/AssetScanner.h>
#include<Editor/Services/IClipboard.h>
#include<Editor/Services/EditorClipboard.h>
#include<Editor/Services/SelectionManager.h>
#include<Editor/UI/Backend/ImGuiRenderer.h>
#include<Editor/UI/Backend/ImGuiInitializer.h>
#include<Editor/UI/Backend/EditorShortcuts.h>
#include<Editor/Commands/EditorCommandRegistry.h>
#include<ranges>
namespace DM
{
	Editor::Editor(const std::string_view& name) :Layer(name)
	{
		
	}

	Editor::~Editor()
	{
		
	}

	void Editor::OnAttach()
	{
		// 服务装配(组合根)：统一创建并注册编辑器服务，替代散落的 static Get() 单例
		EditorContext::Get()->GetServiceRegistry()
			.Register<SelectionManager>()
			.RegisterAs<IClipboard, EditorClipboard>()
			.Register<AssetScanner>();
		EditorContext::Get()->GetServiceRegistry().InitAll();

		// 注册缺包处理回调：引擎加载到未导入资产时按需导入(保持 AssetMgr 只负责加载的职责边界)
		AssetMgr::SetMissingAssetHandler([](const std::string& sourceFilePath) -> bool {
			return EditorContext::GetService<AssetScanner>()->Import(sourceFilePath);
		});

		m_ImGuiRenderer = CreateUPtr<ImGuiRenderer>();
		ImGuiInitializer::Init(m_ImGuiRenderer.get());

		AddPanel<ViewportPanel>();
		AddPanel<MenuBarPanel>();
		AddPanel<SceneHierarchyPanel>();
		AddPanel<PropertiesPanel>();
		AddPanel<ContentBrowserPanel>();
		AddPanel<ImportAssetDialogPanel>();
		AddPanel<StylePanel>();

		// 集中注册编辑器命令元数据(菜单/快捷键/动作三合一)，须先于快捷键与菜单使用
		EditorCommandRegistry::RegisterAll();

		// 集中注册编辑器默认快捷键(菜单命令快捷键由命令注册表驱动)
		EditorShortcuts::Get()->RegisterDefaults();

		// 初始化时扫描资产目录，发现新资源则提示导入
		RequestAssetScan();
	}

	void Editor::OnDetach()
	{

		RHI::RHIDevice::Get()->WaitGPUIdle();

		for (Panel* panel : m_Panels)
		{
			if (panel)delete panel;

		}
		ImGuiInitializer::ShutDown();

		// 服务关闭(逆序)：统一释放编辑器服务
		EditorContext::Get()->GetServiceRegistry().ShutdownAll();
	}

	void Editor::OnUpdate(float DeltaTime)
	{
		// 每次窗口重新聚焦到引擎时扫描一次资产目录
		const bool focused = Engine::Get()->GetAppWindow().IsWindowFocused();
		if (focused && !m_bLastWindowFocused)
		{
			RequestAssetScan();
		}
		m_bLastWindowFocused = focused;

		Begin();
		Render();
		End();
	}

	void Editor::HandleEvent(Event* const e)
	{
		std::ranges::for_each(m_Panels, [&](Panel* panel) {panel->HandleEvent(e); });
	}

	void Editor::Begin()
	{
		m_ImGuiRenderer->Begin();
		// 快捷键统一由注册表检测与分派，不再在帧循环中写死按键
		EditorShortcuts::Get()->Process();
	}

	void Editor::Render()
	{
		for (Panel* panel : m_Panels)
		{
			panel->Render();
		}
	}

	void Editor::End()
	{
		m_ImGuiRenderer->End();
	}


	void Editor::RequestAssetScan()
	{
		auto* scanner = EditorContext::GetService<AssetScanner>();
		auto newAssets = scanner ? scanner->Scan() : std::vector<AssetScanner::NewAsset>{};
		if (newAssets.empty()) return;

		if (auto* panel = GetPanel<ImportAssetDialogPanel>())
		{
			panel->ShowNewAssets(std::move(newAssets));
		}
	}

}
