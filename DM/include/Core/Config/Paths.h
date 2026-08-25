#pragma once
#include <string>
#include <cstdlib>
#include <filesystem>

namespace DM
{
	/// <summary>
	/// 统一路径服务：所有"项目根/配置目录/资产根/缓存目录"都以 ProjectRoot 为唯一基准，
	/// 替代散落的 current_path()/"./Assets" 等相对路径，保证配置与资产位置可预测、可迁移。
	/// 项目根推导顺序：显式参数 -> 环境变量 DM_PROJECT_ROOT -> 当前工作目录(开发期零迁移成本)。
	/// </summary>
	class Paths
	{
	public:
		static void Init(const std::string& projectRoot = "")
		{
			if (s_Initialized && projectRoot.empty())return;

			if (!projectRoot.empty())
			{
				s_ProjectRoot = std::filesystem::absolute(projectRoot).lexically_normal();
			}
			else if (const char* env = std::getenv("DM_PROJECT_ROOT"); env && *env)
			{
				s_ProjectRoot = std::filesystem::absolute(env).lexically_normal();
			}
			else
			{
				s_ProjectRoot = std::filesystem::current_path().lexically_normal();
			}
			s_Initialized = true;
		}

		static const std::filesystem::path& ProjectRoot() { return s_ProjectRoot; }

		/// 集中配置目录：<ProjectRoot>/Config
		static std::filesystem::path ConfigDir() { return s_ProjectRoot / "Config"; }

		/// 指定配置文件的完整路径：<ProjectRoot>/Config/<name>
		static std::filesystem::path ConfigFile(const char* name) { return ConfigDir() / name; }

		/// 资产根目录
		static std::filesystem::path AssetRoot() { return s_ProjectRoot / "Assets"; }

		/// 缓存目录：<ProjectRoot>/Cache
		static std::filesystem::path CacheDir() { return s_ProjectRoot / "Cache"; }

	private:
		static inline std::filesystem::path s_ProjectRoot = std::filesystem::current_path();
		static inline bool s_Initialized = false;
	};
}
