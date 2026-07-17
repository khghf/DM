#include"DMPCH.h"
#include <Core/Log.h>

#include "spdlog/sinks/stdout_color_sinks.h"
namespace DM
{
	std::shared_ptr<spdlog::logger>DM::Log::CoreLogger=nullptr;
	std::shared_ptr<spdlog::logger>DM::Log::ClientLogger=nullptr;
	Log::Log()
	{

	}

	Log::~Log()
	{

	}
	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n:%v%$");
		CoreLogger = spdlog::stderr_color_mt("Core");
		ClientLogger = spdlog::stderr_color_mt("Client");
		CoreLogger->set_level(spdlog::level::trace);
		ClientLogger->set_level(spdlog::level::trace);
	}
}
