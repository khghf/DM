#pragma once
#include"AssetMagicRegistry.h"
#include <fstream>
#include <filesystem>

namespace DM
{
    /// <summary>
    /// 资产文件检测器类，用于读取文件头、通过魔术头检测文件类型，以及获取文件扩展名。
    /// </summary>
    class AssetFileDetector
    {
    public:
        static constexpr size_t HEADER_READ_SIZE = 16;  

        static std::vector<uint8_t> ReadFileHeader(const std::string& path, size_t bytes = HEADER_READ_SIZE)
        {
            std::ifstream file(path, std::ios::binary);
            if (!file.is_open())
            {
                return {};
            }
            return ReadFileHeader(file,bytes);
        }
        static std::vector<uint8_t>ReadFileHeader(std::ifstream& file, size_t bytes = HEADER_READ_SIZE)
        {
            std::vector<uint8_t> header(bytes);
            file.read(reinterpret_cast<char*>(header.data()), bytes);
            return header;
        }

        static const MagicHeaderInfo* DetectByMagic(const std::string& path)
        {
            auto header = ReadFileHeader(path);
            if (header.empty())
            {
                return nullptr;
            }
            return AssetMagicRegistry::Get().FindByMagic(header);
        }

        static std::string GetExtension(const std::string& path)
        {
            std::filesystem::path p(path);
            std::string ext = p.extension().string();
            return ext;
        }
    };
}