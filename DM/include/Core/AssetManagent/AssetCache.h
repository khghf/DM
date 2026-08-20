#pragma once
#include <Foundation/MMM/Reference.h>
#include <unordered_map>
#include <shared_mutex>
#include <type_traits>
#include "AssetObject/AssetObject.h"

namespace DM {

    class AssetCache;
    class DM_API AssetHandle 
    {
    public:
        AssetHandle() = default;
        explicit AssetHandle(const AssetID& id) : m_AssetObjectId(id) {}
        ~AssetHandle() = default;

        AssetHandle(const AssetHandle&) = default;
        AssetHandle& operator=(const AssetHandle&) = default;
        AssetHandle(AssetHandle&&) = default;
        AssetHandle& operator=(AssetHandle&&) = default;

        template<typename T>
        SPtr<T> GetAsset() const;

        SPtr<AssetObject> GetAsset() const;

        bool IsValid() const;

        AssetID GetAssetId() const { return m_AssetObjectId; }

        bool operator==(const AssetHandle& other) const 
        {
            return m_AssetObjectId == other.m_AssetObjectId;
        }

        bool operator!=(const AssetHandle& other) const 
        {
            return !(*this == other);
        }

        explicit operator bool() const 
        {
            return IsValid();
        }

    private:
        AssetID m_AssetObjectId{};
    };

    class DM_API AssetCache
    {
    public:
        AssetCache(const AssetCache&) = delete;
        AssetCache& operator=(const AssetCache&) = delete;

        static AssetCache* Get() 
        {
            static AssetCache instance;
            return &instance;
        }

        void CacheAsset(const AssetID& id, SPtr<AssetObject> asset);

        void CacheAsset(const AssetHandle& handle, SPtr<AssetObject> asset);
        
       /* template<typename T>
        void CacheAsset(const AssetID& id, SPtr<T> asset) const
        {
            CacheAsset(id, std::dynamic_pointer_cast<AssetObject>(asset));
        }
        template<typename T>
        void CacheAsset(const AssetHandle& handle, SPtr<T> asset) const
        {
            CacheAsset<T>(handle.GetAssetId(),asset);
        }*/

        SPtr<AssetObject> GetAsset(const AssetID& id) const;
        SPtr<AssetObject> GetAsset(const AssetHandle& handle) const;

        template<typename T>
        SPtr<T> GetAsset(const AssetID& id) const
        {
            SPtr<AssetObject> asset = GetAsset(id);
            return std::dynamic_pointer_cast<T>(asset);
        }
        template<typename T>
        SPtr<T> GetAsset(const AssetHandle& handle) const
        {
            return GetAsset<T>(handle.GetAssetId());
        }


        void RemoveAsset(const AssetID& id);
        void RemoveAsset(const AssetHandle& handle);

        void RemoveAssetIfExpired(const AssetID& id);
        void RemoveAssetIfExpired(const AssetHandle& handle);


        void CleanupExpired();

        bool IsExpired(const AssetID& id) const;
        bool IsExpired(const AssetHandle& handle) const;

        size_t Size() const;

        void Clear();

    private:
        AssetCache() = default;
        ~AssetCache() = default;

        mutable std::shared_mutex m_Mutex;
        std::unordered_map<AssetID, WPtr<AssetObject>> m_Cache;
    };


    inline bool AssetHandle::IsValid() const 
    {
        return !AssetCache::Get()->IsExpired(m_AssetObjectId);
    }

    inline SPtr<AssetObject> AssetHandle::GetAsset() const 
    {
        return AssetCache::Get()->GetAsset(m_AssetObjectId);
    }

    template<typename T>
    inline SPtr<T> AssetHandle::GetAsset() const 
    {
        auto asset = AssetCache::Get()->GetAsset(m_AssetObjectId);
        if (!asset) 
        {
            return nullptr;
        }

        return std::dynamic_pointer_cast<T>(asset);
    }

} // namespace DM
