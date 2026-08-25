#pragma once
#include <string>
#include <vector>

#include "Editor/Core/Service/IService.h"
namespace DM
{
	/// <summary>剪贴板模式：复制 / 剪切</summary>
	enum class EClipboardMode : uint8_t
	{
		None = 0,
		Copy,
		Cut
	};

	/// <summary>
	/// 剪贴板服务接口：内容浏览器等 UI 仅依赖此接口，
	/// 便于未来替换底层实现(如接入系统剪贴板)而不影响消费方。
	/// </summary>
	class IClipboard:public IService
	{
	public:
		virtual ~IClipboard() = default;

		/// <summary>设置剪贴板内容(路径集合 + 模式)</summary>
		virtual void Set(const std::vector<std::string>& paths, EClipboardMode mode) = 0;

		/// <summary>清空剪贴板</summary>
		virtual void Clear() = 0;

		/// <summary>当前模式</summary>
		virtual EClipboardMode GetMode() const = 0;

		/// <summary>剪贴板中的路径集合</summary>
		virtual const std::vector<std::string>& GetPaths() const = 0;

		/// <summary>是否持有内容</summary>
		virtual bool HasContent() const = 0;

		/// <summary>是否为剪切模式(且有内容)</summary>
		virtual bool IsCut() const = 0;

		/// <summary>是否为复制模式(且有内容)</summary>
		virtual bool IsCopy() const = 0;

		/// <summary>路径是否在剪贴板中(归一化比较)</summary>
		virtual bool Contains(const std::string& absPath) const = 0;

		/// <summary>剪贴板快照(供命令撤销/重做恢复)</summary>
		struct Snapshot
		{
			EClipboardMode Mode = EClipboardMode::None;
			std::vector<std::string> Paths;
		};

		/// <summary>对剪贴板当前内容做快照(供命令撤销/重做)</summary>
		virtual Snapshot SnapshotState() const = 0;

		/// <summary>恢复剪贴板到指定快照</summary>
		virtual void Restore(const Snapshot& snap) = 0;
	};
}
