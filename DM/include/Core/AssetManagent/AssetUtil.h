#pragma once
#include<string_view>
#include<Core/Reflection/Mirror/include/mirror.h>
namespace DM
{
	class AssetPack;
	struct DM_API AssetUtil
	{
		static uint64_t GetFileLastModifyTimeStamp(std::string_view filePath);

		static uint64_t TimeNow();
		static uint64_t GetFileSize(std::string_view filePath);

		static std::string Sha256FileContent(std::string_view filePath);

		static std::string GetExtension(std::string_view path);

		static std::string SerializePack(AssetPack* pack);

		static std::string SerializePack(AssetPack* pack, std::string_view sourceFilePath);

		template<typename Class>
		static void Serialize(const Class* val, std::string_view savePath)
		{
#ifdef DM_DEBUG1
			return mirror::Serialize(savePath, val);
#else
			return mirror::SerializeBinary(savePath, val);
#endif // DM_EDITOR
		}

		template<typename Class>
		static void Deserialize(Class* val, std::string_view path)
		{
#ifdef DM_DEBUG1
			return mirror::Deserialize(path, val);
#else
			return mirror::DeserializeBinary(path, val);
#endif // DM_EDITOR

		}
	};
}