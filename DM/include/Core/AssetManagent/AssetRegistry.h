//#pragma once
//#include<DMPCH.h>
//#include<Core/Core.h>
//#include<filesystem>
//namespace DM
//{
//	template<typename KeyEvent,typename Val>
//	class AssetRegistry
//	{
//		std::unordered_map<KeyEvent, SPtr<Val>>m_Registry;
//	public:
//		bool					HasAssetObject(const KeyEvent& key)const;
//		bool					HasAssetObject(const KeyEvent& key,SPtr<Val>&out)const;
//		SPtr<Val>				GetAssetObject(const KeyEvent& key)const;
//		std::vector<SPtr<Val>>	GetAllAssetObject()const;
//		SPtr<Val>				RegisterAssetObject(const KeyEvent& key, SPtr<Val> val);
//		void					UnregisterAllAssetObject();
//	};
//
//	template<typename KeyEvent, typename Val>
//	inline bool AssetRegistry<KeyEvent, Val>::HasAssetObject(const KeyEvent& key) const
//	{
//		return m_Registry.find(key) != m_Registry.end();
//		
//	}
//
//	template<typename KeyEvent, typename Val>
//	inline bool AssetRegistry<KeyEvent, Val>::HasAssetObject(const KeyEvent& key, SPtr<Val>& out) const
//	{
//		auto it = m_Registry.find(key);
//
//		if (it != m_Registry.end())
//		{
//			out = it;
//			return true;
//		}
//		return false;
//	}
//
//	template<typename KeyEvent, typename Val>
//	inline SPtr<Val> AssetRegistry<KeyEvent, Val>::GetAssetObject(const KeyEvent& key) const
//	{
//		SPtr<Val> res;
//		if (HasAsset(key))
//		{
//			res = m_Registry.at(key);
//		}
//		return res;
//	}
//
//	template<typename KeyEvent, typename Val>
//	inline std::vector<SPtr<Val>> AssetRegistry<KeyEvent, Val>::GetAllAssetObject() const
//	{
//		std::vector<SPtr<Val>>res = { m_Registry.begin(),m_Registry.end() };
//		return res;
//	}
//
//	template<typename KeyEvent, typename Val>
//	inline SPtr<Val> AssetRegistry<KeyEvent, Val>::RegisterAssetObject(const KeyEvent& key, SPtr<Val> val)
//	{
//		SPtr<Val> res;
//		if (HasAsset(key, res))
//		{
//			return res;
//		}
//		return SPtr<Val>();
//	}
//
//	template<typename KeyEvent, typename Val>
//	inline void AssetRegistry<KeyEvent, Val>::UnregisterAllAssetObject()
//	{
//		m_Registry.swap({});
//	}
//}
//
//
