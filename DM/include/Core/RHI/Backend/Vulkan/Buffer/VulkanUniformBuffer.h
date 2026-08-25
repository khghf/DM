#pragma once
#include"VulkanBuffer.h"
namespace DM::RHI
{
	class VulkanUniformBuffer:public VulkanBuffer
	{
		friend class VulkanShaderProgram;
	public:
		VulkanUniformBuffer(VulkanDevice*device,const RHIUniformBufferDesc&desc);
		~VulkanUniformBuffer();

		virtual void Update(const void* data, size_t size) override;
		virtual EResourceType GetResourceType() const { return EResourceType::UniformBuffer; }

	private:
		bool m_bDynamic;
	};
}