#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <algorithm>

namespace DM
{
    struct MagicHeaderInfo
    {
        std::vector<uint8_t> MagicBytes;    // 魔数字节序列
        std::string Extension;              // 对应的扩展名

        bool Matches(const std::vector<uint8_t>& data) const
        {
            if (data.size() < MagicBytes.size()) return false;
            return std::equal(MagicBytes.begin(), MagicBytes.end(), data.begin());
        }
    };
    /// <summary>
    /// 管理和注册文件扩展名与其对应的魔术头信息的注册表。
    /// </summary>
    class AssetMagicRegistry
    {
    public:
        using Index = uint16_t;
        static constexpr Index INVALID_INDEX = UINT16_MAX;

        static AssetMagicRegistry& Get()
        {
            static AssetMagicRegistry instance;
            return instance;
        }

        template<typename... Bytes>
        Index Register(const std::string& extension, Bytes... bytes)
        {
            MagicHeaderInfo info;
            info.Extension = extension;
            info.MagicBytes = { static_cast<uint8_t>(bytes)... };

            Index idx = static_cast<Index>(m_MagicList.size());
            m_MagicList.push_back(std::move(info));
            m_ExtensionToIndex[extension] = idx;

            // 标记需要重新排序
            m_NeedSort = true;

            return idx;
        }

        void Sort()
        {
            if (!m_NeedSort) return;

            // 按魔术头长度降序排序(长的优先匹配)
            std::sort(m_MagicList.begin(), m_MagicList.end(),
                [](const MagicHeaderInfo& a, const MagicHeaderInfo& b) {
                    if (a.MagicBytes.size() != b.MagicBytes.size())
                    {
                        return a.MagicBytes.size() > b.MagicBytes.size();
                    }
                    return a.MagicBytes < b.MagicBytes;
                });

            m_ExtensionToIndex.clear();
            for (Index i = 0; i < static_cast<Index>(m_MagicList.size()); ++i)
            {
                m_ExtensionToIndex[m_MagicList[i].Extension] = i;
            }

            m_NeedSort = false;
        }

        const MagicHeaderInfo* GetByIndex(Index idx) const
        {
            if (idx >= m_MagicList.size()) return nullptr;
            return &m_MagicList[idx];
        }

        Index FindIndexByExtension(const std::string& extension) const
        {
            const_cast<AssetMagicRegistry*>(this)->Sort();

            auto it = m_ExtensionToIndex.find(extension);
            return it != m_ExtensionToIndex.end() ? it->second : INVALID_INDEX;
        }

        const MagicHeaderInfo* FindByExtension(const std::string& extension) const
        {
            Index idx = FindIndexByExtension(extension);
            return GetByIndex(idx);
        }

        Index FindIndexByMagic(const std::vector<uint8_t>& header) const
        {
            const_cast<AssetMagicRegistry*>(this)->Sort();

            for (Index i = 0; i < static_cast<Index>(m_MagicList.size()); ++i)
            {
                if (m_MagicList[i].Matches(header))
                {
                    return i;
                }
            }
            return INVALID_INDEX;
        }

        const MagicHeaderInfo* FindByMagic(const std::vector<uint8_t>& header) const
        {
            Index idx = FindIndexByMagic(header);
            return GetByIndex(idx);
        }

        const std::vector<MagicHeaderInfo>& GetAll() const
        {
            const_cast<AssetMagicRegistry*>(this)->Sort();
            return m_MagicList;
        }

        size_t Size() const { return m_MagicList.size(); }

        void Clear()
        {
            m_MagicList.clear();
            m_ExtensionToIndex.clear();
            m_NeedSort = false;
        }

    private:
        AssetMagicRegistry()
        {
            auto& registry = *this;

            // 图片格式
            registry.Register(".png", 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A);
            registry.Register(".jpg", 0xFF, 0xD8, 0xFF);
            registry.Register(".jpeg", 0xFF, 0xD8, 0xFF);
            registry.Register(".tga", 0x00, 0x00, 0x02, 0x00);
            registry.Register(".bmp", 0x42, 0x4D);
            registry.Register(".gif", 0x47, 0x49, 0x46, 0x38);
            registry.Register(".webp", 0x52, 0x49, 0x46, 0x46);
            registry.Register(".dds", 0x44, 0x44, 0x53, 0x20);
            registry.Register(".psd", 0x38, 0x42, 0x50, 0x53);

            // 3D 模型格式
            registry.Register(".fbx", 0x46, 0x42, 0x58);
            registry.Register(".glb", 0x67, 0x6C, 0x54, 0x46);
            registry.Register(".dae", 0x3C, 0x3F, 0x78, 0x6D, 0x6C);
            registry.Register(".3ds", 0x4D, 0x4D);
            registry.Register(".obj", 0x23, 0x20);
            registry.Register(".stl", 0x73, 0x6F, 0x6C, 0x69, 0x64);
            registry.Register(".gltf", 0x7B, 0x22);

            // 音频格式
            registry.Register(".wav", 0x52, 0x49, 0x46, 0x46);
            registry.Register(".mp3", 0xFF, 0xFB);
            registry.Register(".ogg", 0x4F, 0x67, 0x67, 0x53);
            registry.Register(".flac", 0x66, 0x4C, 0x61, 0x43);
            registry.Register(".aiff", 0x46, 0x4F, 0x52, 0x4D);

            // 视频格式
            registry.Register(".mp4", 0x00, 0x00, 0x00, 0x18, 0x66, 0x74, 0x79, 0x70);
            registry.Register(".avi", 0x52, 0x49, 0x46, 0x46, 0x00, 0x00, 0x00, 0x00, 0x41, 0x56, 0x49, 0x20);
            registry.Register(".mov", 0x6D, 0x6F, 0x6F, 0x76);
            registry.Register(".mkv", 0x1A, 0x45, 0xDF, 0xA3);
            registry.Register(".webm", 0x1A, 0x45, 0xDF, 0xA3);

            // 字体格式
            registry.Register(".ttf", 0x00, 0x01, 0x00, 0x00, 0x00, 0x00);
            registry.Register(".otf", 0x4F, 0x54, 0x54, 0x4F);
            registry.Register(".woff", 0x77, 0x4F, 0x46, 0x46);
            registry.Register(".woff2", 0x77, 0x4F, 0x46, 0x32);
            registry.Register(".ttc", 0x74, 0x74, 0x63, 0x66);

            // 压缩格式
            registry.Register(".zip", 0x50, 0x4B, 0x03, 0x04);
            registry.Register(".gz", 0x1F, 0x8B);
            registry.Register(".7z", 0x37, 0x7A, 0xBC, 0xAF, 0x27, 0x1C);
            registry.Register(".rar", 0x52, 0x61, 0x72, 0x21);

            // .dasset(自定义格式)
            registry.Register(".dasset", 0x44, 0x41, 0x53, 0x53);

            // 注册完成后排序
            registry.Sort();
        }
        std::vector<MagicHeaderInfo> m_MagicList;
        std::unordered_map<std::string, Index> m_ExtensionToIndex;
        bool m_NeedSort = false;
    };

    template<typename... Bytes>
    inline AssetMagicRegistry::Index RegisterMagicHeader(const std::string& extension, Bytes... bytes)
    {
        return AssetMagicRegistry::Get().Register(extension, bytes...);
    }
}