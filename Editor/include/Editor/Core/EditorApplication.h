#pragma once
#include<Core/Layer/Layer.h>
#include<Editor/Defines.h>
#include"Editor/UI/Panels/ViewportPanel.h"
#include"Editor/UI/Panels/MenuBarPanel.h"
#include<vector>
namespace DM
{
	class ImGuiRenderer;
	class Panel;
	/// <summary>
	/// 编辑器入口层：只负责面板生命周期(创建/渲染/事件分发)与启动装配(服务/命令/快捷键)。
	/// 世界状态与切换(编辑世界/播放世界/活动世界)统一由 EditorContext 管理。
	/// </summary>
	class EDITOR_API Editor :public Layer
	{
		friend class APP;

		Editor(const std::string_view& name);
	public:
		virtual ~Editor();
		virtual void OnAttach()override;
		
		virtual void OnDetach()override;
		
		virtual void OnUpdate(float DeltaTime)override;
		
		virtual void HandleEvent(DM::Event* const e)override;

		static Editor* Get()
		{
			static Editor* inst = new Editor("Editor");
			return inst;
		}

		/// <summary>
		/// 按类型获取面板实例
		/// </summary>
		template<typename T>
		T* GetPanel()const
		{
			for (Panel* panel : m_Panels)
			{
				if (T* typed = dynamic_cast<T*>(panel)) return typed;
			}
			return nullptr;
		}

	protected:
		void Begin();
		void Render();
		void End();


		template<typename T,typename...Args>
		int AddPanel(Args&&...arg) { m_Panels.emplace_back(new T{std::forward<Args>(arg)...}); return m_Panels.size() - 1; }
	
	private:
		/// <summary>
		/// 扫描资产目录，发现新可导入资源则交给导入提示面板
		/// </summary>
		void RequestAssetScan();
	private:
		std::vector<Panel*>m_Panels;

		UPtr<ImGuiRenderer>m_ImGuiRenderer;

		bool m_bLastWindowFocused = false;  // 上一帧窗口焦点状态(用于检测聚焦边沿)
	};
}
