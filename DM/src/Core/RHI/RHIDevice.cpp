#include"Core/RHI/RHIDevice.h"
#include"Core/RHI/Backend/Vulkan/VulkanDevice.h"
#include<iostream>

namespace DM::RHI
{
	RHIDevice* RHIDevice::m_Inst = nullptr;

	void RHIDevice::Init(const RHIDeviceDesc& desc)
	{
		if (m_Inst == nullptr)
		{
			switch (desc.ApiType)
			{
			case EAPIType::Vulkan:
				RHIDevice::m_Inst = new VulkanDevice(desc);
				break;
			case EAPIType::OpenGL:
				std::cerr << "[RHI] OpenGL backend not implemented yet\n";
				RHIDevice::m_Inst = nullptr;
				break;
			default:
				std::cerr << "[RHI] unknown API type\n";
				RHIDevice::m_Inst = nullptr;
				break;
			}
		}
		m_Inst->m_WindowHandle = desc.WindowHandle;

	}

	void RHIDevice::ShutDown()
	{
		m_Inst->WaitGPUIdle();
		delete m_Inst;
	}
}
