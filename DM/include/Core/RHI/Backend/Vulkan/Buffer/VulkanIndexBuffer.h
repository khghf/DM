#pragma once
#include<Core/RHI/RHITypes.h>
#include"VulkanBuffer.h"
#include<vulkan/vulkan.hpp>
namespace DM::RHI
{
	class VulkanDevice;
	class VulkanIndexBuffer:public VulkanBuffer
	{
		friend class VulkanCommandList;
	public:
		VulkanIndexBuffer(VulkanDevice*device,const RHIIndexBufferDesc&desc);
		~VulkanIndexBuffer();
		virtual EResourceType GetAssetType() const { return EResourceType::Indexbuffer; }
		virtual void UpdateData(const void* data, size_t size) override {};
	protected:
	private:

	};
}