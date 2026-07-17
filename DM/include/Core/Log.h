#pragma once
#include"DMPCH.h"
#include<spdlog/spdlog.h>
namespace DM
{
	class DM_API Log
	{
	public:
		Log();
		~Log();
		static void Init();
		inline static std::shared_ptr<spdlog::logger>GetCoreLogger() { return CoreLogger; };
		inline static std::shared_ptr<spdlog::logger>GetClientLogger() { return ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger>CoreLogger;
		static std::shared_ptr<spdlog::logger>ClientLogger;
	};
}
#define LOG_CORE_TRACE(...)	::DM::Log::GetCoreLogger()->trace		(__VA_ARGS__)
#define LOG_CORE_INFO(...)	::DM::Log::GetCoreLogger()->info		(__VA_ARGS__)
#define LOG_CORE_WARN(...)	::DM::Log::GetCoreLogger()->warn		(__VA_ARGS__)
#define LOG_CORE_ERROR(...)	::DM::Log::GetCoreLogger()->error		(__VA_ARGS__)
#define LOG_CORE_FATAL(...)	::DM::Log::GetCoreLogger()->critical	(__VA_ARGS__)

#define LOG_TRACE(...)		::DM::Log::GetClientLogger()->trace		(__VA_ARGS__)
#define LOG_INFO(...)		::DM::Log::GetClientLogger()->info		(__VA_ARGS__)
#define LOG_WARN(...)		::DM::Log::GetClientLogger()->warn		(__VA_ARGS__)
#define LOG_ERROR(...)		::DM::Log::GetClientLogger()->error		(__VA_ARGS__)
#define LOG_FATAL(...)		::DM::Log::GetClientLogger()->critical	(__VA_ARGS__)


#define DM_ENABLE_ASSERT
#ifdef DM_ENABLE_ASSERT

#define DM_ASSERT_INTERNAL(x,tip,fmt,file,line,...)\
{\
	if (!(x))\
	{\
		LOG_ERROR(tip##fmt,#file, #line, __VA_ARGS__); \
		__debugbreak(); \
	}\
}

#define DM_CORE_ASSERT_INTERNAL(x,tip,fmt,file,line,...)\
{\
	if (!(x))\
	{\
		LOG_CORE_ERROR(tip##fmt,#file, #line, __VA_ARGS__); \
		__debugbreak(); \
	}\
}
#define DM_ASSERT(x,fmt,...)\
			DM_ASSERT_INTERNAL(x,"Assertion Failed.File:{}---Line:{}.",fmt,__FILE__,__LINE__,__VA_ARGS__)

#define DM_CORE_ASSERT(x,fmt,...)\
			DM_ASSERT_INTERNAL(x,"Assertion Failed.File:{}---Line:{}.",fmt,__FILE__,__LINE__,__VA_ARGS__)
#else
#define DM_ASSERT(x,...)
#define DM_CORE_ASSERT(x,fmt,...)
#endif // DM_ENABLE_ASSERT
