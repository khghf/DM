#pragma once

namespace DM
{
	/// <summary>
	/// 编辑器服务基类：所有编辑器服务(选中/剪贴板/资产扫描等)统一生命周期。
	/// 服务由 ServiceRegistry 统一注册、Init、Shutdown 与销毁，
	/// </summary>
	class IService
	{
	public:
		virtual ~IService() = default;

		virtual void Init() {}

		virtual void Shutdown() {}
	};
}
