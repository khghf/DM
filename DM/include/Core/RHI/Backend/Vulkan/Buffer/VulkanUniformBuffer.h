#pragma once
#include"VulkanBuffer.h"
namespace DM::RHI
{
	extern uint8_t CURRENT_CPU_PROCESSES_FRAME_INDEX;
	class VulkanUniformBuffer:public VulkanBuffer
	{
		friend class VulkanShaderProgram;
	public:
		VulkanUniformBuffer(VulkanDevice*device,const RHIUniformBufferDesc&desc);
		~VulkanUniformBuffer();

		virtual void UpdateData(const void* data, size_t size) override;
		virtual EResourceType GetAssetType() const { return EResourceType::UniformBuffer; }

	private:
		bool m_bDynamic;
	};
}