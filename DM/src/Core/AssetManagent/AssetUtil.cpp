#include<Core/AssetManagent/AssetUtil.h>
#include<filesystem>
#include<fstream>
#include<PicoSHA2/picosha2.h>
#include<Core/AssetManagent/AsetPack/AssetPack.h>
#include<Core/AssetManagent/AssetMetaDatabase.h>
namespace DM
{
	uint64_t AssetUtil::GetFileLastModifyTimeStamp(std::string_view filePath)
	{
		auto lastWriteTime = std::filesystem::last_write_time({ filePath });
		return std::chrono::duration_cast<std::chrono::seconds>(lastWriteTime.time_since_epoch()).count();
	}
	
	uint64_t AssetUtil::TimeNow()
	{
		auto timeNow = std::chrono::system_clock::now();
		return std::chrono::duration_cast<std::chrono::seconds>(timeNow.time_since_epoch()).count();
	}

	uint64_t AssetUtil::GetFileSize(std::string_view filePath)
	{
		std::filesystem::path path(filePath);
		if (!std::filesystem::exists(path))
		{
			throw std::runtime_error("File does not exist: " + std::string(filePath));
		}
		auto size = std::filesystem::file_size(path);
		return  static_cast<uint64_t>(size);
	}

	std::string AssetUtil::Sha256FileContent(std::string_view filePath)
	{
		std::ifstream in(filePath.data(), std::ios::in | std::ios::binary);
		if (!in.is_open())
		{
			throw std::runtime_error("Failed to open file: " + std::string(filePath));
		}

		// 分块哈希
		picosha2::hash256_one_by_one hasher;
		constexpr size_t BUFFER_SIZE = 1024 * 1024;  // 1MB
		std::vector<char> buffer(BUFFER_SIZE);

		while (in.read(buffer.data(), BUFFER_SIZE) || in.gcount() > 0)
		{
			size_t bytesRead =  static_cast<size_t>(in.gcount());
			hasher.process(buffer.begin(), buffer.begin() + bytesRead);
		}

		if (in.fail() && !in.eof())
		{
			throw std::runtime_error("Failed to read file: " + std::string(filePath));
		}

		std::string sha;
		hasher.finish();
		picosha2::get_hash_hex_string(hasher, sha);
		return sha;
	}

	std::string AssetUtil::GetExtension(std::string_view path)
	{
		std::filesystem::path p{ path };
		return p.extension().string();
	}


	std::string AssetUtil::SerializePack(AssetPack* pack)
	{
		return SerializePack(pack, pack->GetMeta().m_SourceFilePath);
	}

	std::string AssetUtil::SerializePack(AssetPack* pack, std::string_view sourceFilePath)
	{
		std::filesystem::path path{ sourceFilePath };
		if (pack->GetExtension() != AssetUtil::GetExtension(sourceFilePath))
		{
			path += pack->GetExtension();
		}

		AssetUtil::Serialize(pack, path.string());
		auto database = AssetMetaDatabase::Get();
		database->AddNewAssetPack(pack);
		database->Save();
		return path.string();
	}
}