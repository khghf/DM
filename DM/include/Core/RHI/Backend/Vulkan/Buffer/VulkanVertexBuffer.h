#pragma once
#include"VulkanBuffer.h"
#include<vulkan/vulkan.h>
namespace DM::RHI
{
	class VulkanDevice;
	/// <summary>
	/// vulkan顶点缓冲对象，构建即将顶点数据传到GPU可读的内存区域
	/// </summary>
	class VulkanVertexBuffer :public VulkanBuffer
	{
		friend class VulkanCommandList;
	public:
		VulkanVertexBuffer(VulkanDevice* device,const RHIVertexBufferDesc&desc);
		~VulkanVertexBuffer()override;
		virtual EResourceType GetResourceType() const { return EResourceType::VertexBuffer; }

		virtual void Update(const void* data, size_t size) override {};
	private:
	private:
		bool HasUploadData = false;
	};
}