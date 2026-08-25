#include <Editor/UI/EditorUI.h>
#include<Editor/UI/Widgets/DragItem.h>
namespace DM
{
	namespace
	{
		// payload 数据布局：[PayloadHeader][AssetID hex '\0'][SourceFilePath '\0'(可选)]
		struct PayloadHeader
		{
			uint32_t Version = 0;
			uint32_t SubType = 0;
		};

		constexpr uint32_t PayloadVersion = 1;
	}

	std::string DragItem::AssetPayloadType = "DM_ASSET_DRAG";

	DragItem::DragItem(AssetID id, EAssetType type, std::string path)
		: AssetId(std::move(id))
		, Type(type)
		, SourceFilePath(std::move(path))
	{
	}

	void DragItem::SetDragDropPayload() const
	{
		if (!IsValid())
		{
			return;
		}

		PayloadHeader header;
		header.Version = PayloadVersion;
		header.SubType = static_cast<uint32_t>(Type);

		const std::string id = AssetId.GetID();

		std::vector<char> buffer(sizeof(header));
		std::memcpy(buffer.data(), &header, sizeof(header));

		buffer.insert(buffer.end(), id.begin(), id.end());
		buffer.push_back('\0');

		if (!SourceFilePath.empty())
		{
			buffer.insert(buffer.end(), SourceFilePath.begin(), SourceFilePath.end());
			buffer.push_back('\0');
		}

		EditorUI::SetDragDropPayload(AssetPayloadType.c_str(), buffer.data(), buffer.size());
	}

	DragItem DragItem::FromPayload(const ImGuiPayload* payload)
	{
		DragItem item;
		if (payload == nullptr || payload->Data == nullptr || payload->DataSize < sizeof(PayloadHeader))
		{
			return item;
		}

		const char* cursor = static_cast<const char*>(payload->Data);
		const char* end = cursor + payload->DataSize;

		PayloadHeader header;
		std::memcpy(&header, cursor, sizeof(header));
		cursor += sizeof(header);

		// 版本不匹配：拒绝，避免跨版本误读
		if (header.Version != PayloadVersion)
		{
			return item;
		}

		item.Type = static_cast<EAssetType>(header.SubType);

		// AssetID hex 字符串
		std::string id;
		while (cursor < end && *cursor != '\0')
		{
			id.push_back(*cursor++);
		}
		if (id.empty() || cursor >= end)
		{
			return item;
		}
		++cursor; // 跳过结尾 '\0'

		item.AssetId = AssetID(std::move(id));

		// 剩余字段为 SourceFilePath(可选)
		if (cursor < end)
		{
			std::string path;
			while (cursor < end && *cursor != '\0')
			{
				path.push_back(*cursor++);
			}
			item.SourceFilePath = std::move(path);
		}

		return item;
	}
}
