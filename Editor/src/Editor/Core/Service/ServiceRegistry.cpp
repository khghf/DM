#include "Editor/Core/Service/ServiceRegistry.h"

namespace DM
{
	void ServiceRegistry::InitAll()
	{
		for (auto& [key, service] : m_Services)
		{
			if (service)
			{
				service->Init();
			}
		}
	}

	void ServiceRegistry::ShutdownAll()
	{
		// 逆序调用 Shutdown(依赖后注册的服务先关闭)
		for (auto it = m_Services.end(); it != m_Services.begin();)
		{
			--it;
			if (it->second)
			{
				it->second->Shutdown();
			}
		}
	}
}
