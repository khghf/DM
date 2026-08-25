#pragma once
#include <string>
#include <Core/AssetManagent/AsetPack/AssetPack.h>

struct ImGuiPayload;

namespace DM
{
	class DragItem
	{
	public:
		static std::string AssetPayloadType;

	public:
		AssetID AssetId; 
		EAssetType Type = EAssetType::Unknown;
		std::string SourceFilePath; 

		DragItem() = default;
		explicit DragItem(AssetID id, EAssetType type, std::string path);

		bool IsValid() const { return AssetId.IsValid(); }

		void SetDragDropPayload() const;

		static DragItem FromPayload(const ImGuiPayload* payload);
	};
}
