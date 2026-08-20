#pragma once
#include<Core/Core.h>

#include"../AssetUtil.h"
#include<Foundation/MMM/Reference.h>
#include<chrono>
#include<filesystem>
#include<random>
#include<Core/Log.h>
#include<Core/Object.h>
#include<PicoSHA2/picosha2.h>
namespace DM
{
	class AssetPack;
	

	enum class EAssetType:uint8_t
	{
		Unknown,
		Shader,
		Texture2D,
		Model,
		World,
	};
	REGISTER_ENUM_ITEM(EAssetType::Unknown);
	REGISTER_ENUM_ITEM(EAssetType::Shader);
	REGISTER_ENUM_ITEM(EAssetType::Texture2D);
	REGISTER_ENUM_ITEM(EAssetType::Model);

	struct DM_API AssetID
	{
		using InnerID = std::string;
		friend class AssetIDGen;
		bool operator==(const AssetID& other)const
		{
			return this->m_ID == other.m_ID;
		}

		bool operator!=(const AssetID& other)const
		{
			return !(*this == other);
		}

		bool IsValid()const
		{
			return m_ID != m_InvalidID;
		}

		explicit operator bool()const
		{
			return IsValid();
		}


		InnerID GetID()const { return m_ID; }
		static AssetID GenNewID()
		{
			AssetID id{};
			static std::atomic<uint64_t> counter{ 0 };

			auto now = std::chrono::system_clock::now();

			auto timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();

			std::random_device rd;
			std::mt19937_64 gen(rd());
			uint64_t random = gen();
			uint64_t count = counter.fetch_add(1);

			std::string combined = std::to_string(timestamp) + std::to_string(random) + std::to_string(count);
			picosha2::hash256_hex_string(combined, id.m_ID);
			return id;
		}

	private:
		static InnerID m_InvalidID;
		InnerID m_ID= m_InvalidID;
	};

	REGISTER_TYPE(AssetID);
	REGISTER_PRIVATE_MEMBER(AssetID, m_ID);

	
	struct DM_API AssetMetaInfo {
		// 版本信息
		uint32_t m_Version = 1;
		// 资产类型
		EAssetType m_Type = EAssetType::Unknown;

		//源文件内容哈希(用于去重和验证)
		std::string m_SourceFileContentHash;
		std::string m_SourceFilePath;

		// 数据大小
		uint64_t m_DataSize = 0;

		// 源文件信息
		uint64_t m_SourceFileSize = 0;

		uint64_t m_SourceFileLastModifyTime = 0;  // 毫秒时间戳

		std::vector<AssetID>m_Dependency = {};//依赖的资产Id
	};

	REGISTER_TYPE(AssetMetaInfo);
	REGISTER_MEMBER(AssetMetaInfo, m_Version);
	REGISTER_MEMBER(AssetMetaInfo, m_Type);
	REGISTER_MEMBER(AssetMetaInfo, m_SourceFileContentHash);
	REGISTER_MEMBER(AssetMetaInfo, m_SourceFilePath);
	REGISTER_MEMBER(AssetMetaInfo, m_DataSize);
	REGISTER_MEMBER(AssetMetaInfo, m_SourceFileSize);
	REGISTER_MEMBER(AssetMetaInfo, m_SourceFileLastModifyTime);
	REGISTER_MEMBER(AssetMetaInfo, m_Dependency);

	//using AssetBody = std::vector<uint8_t>;


	/// <summary>
	/// 表示一个资产包，提供对资产元数据、资产数据和相关信息的访问。
	/// 只会在创建资产对象是才会加载到内存中创建结束后立即卸载
	/// </summary>
	struct DM_API AssetPack
	{
		friend class AssetPackSetter;
	public:
		AssetPack() = default;
		virtual ~AssetPack() = default;

		AssetPack(const AssetPack&) = delete;
		AssetPack& operator=(const AssetPack&) = delete;
		AssetPack(AssetPack&&) = default;
		AssetPack& operator=(AssetPack&&) = default;

		// 获取ID
		AssetID GetGUID() const { return m_GUID; }

		// 元数据访问
		const AssetMetaInfo& GetMeta() const { return m_Meta; }

		// 获取类型
		virtual EAssetType GetAssetType() const = 0;
		virtual std::string GetExtension() const { return ".dasset"; }

		// 检查是否有效
		virtual bool IsValid() const { return m_GUID.IsValid(); }

		AssetMetaInfo GenerateMetaInfo(std::string_view sourceFilePath)const
		{
			AssetMetaInfo metaInfo{};
			metaInfo.m_Type = GetAssetType();
			metaInfo.m_SourceFileContentHash = AssetUtil::Sha256FileContent(sourceFilePath);
			metaInfo.m_SourceFilePath = sourceFilePath.data();
			metaInfo.m_SourceFileLastModifyTime = AssetUtil::GetFileLastModifyTimeStamp(metaInfo.m_SourceFilePath);
			metaInfo.m_SourceFileSize = AssetUtil::GetFileSize(sourceFilePath);
			return metaInfo;
		}

	protected:
		AssetID m_GUID;
		AssetMetaInfo m_Meta;
	};
	REGISTER_TYPE(AssetPack);
	REGISTER_PRIVATE_MEMBER(AssetPack, m_GUID);
	REGISTER_PRIVATE_MEMBER(AssetPack, m_Meta);
	

	struct AssetPackSetter
	{
		static void SetGuid(AssetPack* obj, const AssetID& guid)
		{
			if (obj == nullptr)return;
			obj->m_GUID = guid;
		}
		static void SetMetaInfo(AssetPack* obj, const AssetMetaInfo& metaInfo)
		{
			if (obj == nullptr)return;
			obj->m_Meta = metaInfo;
		}
		static void SetGuid(AssetPack* obj, AssetID&& guid)
		{
			if (obj == nullptr)return;
			obj->m_GUID = guid;
		}
		static void SetMetaInfo(AssetPack* obj, AssetMetaInfo&& metaInfo)
		{
			if (obj == nullptr)return;
			obj->m_Meta = metaInfo;
		}
	};
}


namespace std
{
	template<>
	struct hash<DM::AssetID>
	{
		size_t operator()(const DM::AssetID& id) const noexcept
		{
			return std::hash<std::string>()(id.GetID());
		}
	};
}