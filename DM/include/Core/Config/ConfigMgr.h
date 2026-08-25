#pragma once
#include <typeindex>
#include <unordered_map>
#include <functional>
#include <string>
#include <filesystem>
#include <Core/Reflection/Mirror/include/mirror.h>
#include <Core/Config/Paths.h>

namespace DM
{
	/// <summary>
	/// 统一外部配置文件管理：按类型注册 <T, 文件名>，启动时统一加载，退出时统一保存。
	/// 所有文件位于 Paths::ConfigDir()(<ProjectRoot>/Config) 下，序列化走 Mirror JSON。
	/// 各配置模块通过 ConfigRegistrar<T> 自动注册。
	/// </summary>
	class ConfigMgr
	{
	public:
		struct Entry
		{
			std::string FileName;
			std::function<void(const std::string&)> Load;
			std::function<void(const std::string&)> Save;
		};

		/// 加载所有已注册配置(创建 Config/ 目录，并统一资产根目录为绝对路径)
		static void Init()
		{
			if (s_Inited) return;
			s_Inited = true;

			std::filesystem::create_directories(Paths::ConfigDir());

			for (auto& [type, entry] : Registry())entry.Load((Paths::ConfigDir() / entry.FileName).string());
		}

		/// 保存所有已注册配置到磁盘(引擎退出时调用)
		static void Shutdown()
		{
			for (auto& [type, entry] : Registry())entry.Save((Paths::ConfigDir() / entry.FileName).string());
		}

		/// 访问指定配置类型的运行时单例
		template <typename T>
		static T& Get()
		{
			static T instance{};
			return instance;
		}

		/// 注册一个配置类型及其文件名(通常由 ConfigRegistrar 自动调用)
		template <typename T>
		static void Register(const char* fileName)
		{
			Entry entry;
			entry.FileName = fileName;
			entry.Load = [](const std::string& path) { LoadImpl<T>(path); };
			entry.Save = [](const std::string& path) { SaveImpl<T>(path); };
			Registry()[std::type_index(typeid(T))] = std::move(entry);
		}

	private:
		/// 加载：先重置默认，再覆盖文件中存在的字段(缺失字段/解析失败均保持默认)
		template <typename T>
		static void LoadImpl(const std::string& path)
		{
			T& s = Get<T>();
			s = T{};
			if (std::filesystem::exists(path))mirror::Deserialize(path, &s);
		}

		template <typename T>
		static void SaveImpl(const std::string& path)
		{
			mirror::Serialize(path, &Get<T>());
		}

		static std::unordered_map<std::type_index, Entry>& Registry()
		{
			static std::unordered_map<std::type_index, Entry> s_Registry;
			return s_Registry;
		}

		static inline bool s_Inited = false;
	};

	
	template <typename T>
	struct ConfigRegistrar
	{
		ConfigRegistrar(const char* fileName) { ConfigMgr::Register<T>(fileName); }
	};
}
