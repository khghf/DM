#include<Core/AssetManagent/AssetCache.h>
namespace DM
{
    void AssetCache::CacheAsset(const AssetID& id, SPtr<AssetObject> asset)
    {
        std::unique_lock<std::shared_mutex> lock(m_Mutex);
        if (asset)
        {
            m_Cache[id] = asset;
        }
        else
        {
            RemoveAsset(id);
        }
    }

    void AssetCache::CacheAsset(const AssetHandle& handle, SPtr<AssetObject> asset)
    {
        CacheAsset(handle.GetAssetId(), asset);
    }

    SPtr<AssetObject> AssetCache::GetAsset(const AssetID& id) const
    {
        std::shared_lock<std::shared_mutex> lock(m_Mutex);
        auto it = m_Cache.find(id);
        if (it != m_Cache.end())
        {
            return it->second.lock();

        }
        return nullptr;
    }

    SPtr<AssetObject> AssetCache::GetAsset(const AssetHandle& handle) const
    {
        return GetAsset(handle.GetAssetId());
    }

    void AssetCache::RemoveAsset(const AssetID& id)
    {
        std::unique_lock<std::shared_mutex> lock(m_Mutex);
        m_Cache.erase(id);
    }

    void AssetCache::RemoveAsset(const AssetHandle& handle)
    {
        RemoveAsset(handle.GetAssetId());
    }

    void AssetCache::RemoveAssetIfExpired(const AssetID& id)
    {
        if (IsExpired(id))RemoveAsset(id);
    }

    void AssetCache::RemoveAssetIfExpired(const AssetHandle& handle)
    {
        if (IsExpired(handle))RemoveAsset(handle);
    }

    void AssetCache::CleanupExpired()
    {
        std::unique_lock<std::shared_mutex> lock(m_Mutex);
        for (auto it = m_Cache.begin(); it != m_Cache.end(); )
        {
            if (it->second.expired())
            {
                it = m_Cache.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    bool AssetCache::IsExpired(const AssetID& id) const
    {
        std::shared_lock<std::shared_mutex> lock(m_Mutex);
        auto it = m_Cache.find(id);
        if (it != m_Cache.end())
        {
            return it->second.expired();
        }
        return true;
    }

    bool AssetCache::IsExpired(const AssetHandle& handle) const
    {
        return IsExpired(handle.GetAssetId());
    }

    size_t AssetCache::Size() const
    {
        std::shared_lock<std::shared_mutex> lock(m_Mutex);
        return m_Cache.size();
    }

    void AssetCache::Clear()
    {
        std::unique_lock<std::shared_mutex> lock(m_Mutex);
        m_Cache.clear();
    }
}