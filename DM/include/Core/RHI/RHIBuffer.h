#pragma once

#include"Core/RHI/RHIResource.h"
#include"Core/RHI/RHITypes.h"

namespace DM::RHI
{
	/// <summary>
	/// 缓冲抽象（顶点 / 索引 / Uniform / 读写存储缓冲的统一基类）。
	/// 上层只通过 RHIBuffer* 使用，不感知后端是 VkBuffer 还是 GL VBO。
	/// </summary>
	class DM_API RHIBuffer : public RHIResource
	{
	public:
		virtual ~RHIBuffer() = default;

		/// <summary>将缓冲显存映射到 CPU 可见地址（Vulkan: vkMapMemory）。</summary>
		/// <returns>可写的 CPU 指针；不可映射（如只给 GPU 用的显存）时返回 nullptr。</returns>
		virtual void* Map() = 0;

		/// <summary>解除映射（Vulkan: vkUnmapMemory）。写完后必须调用，否则 GPU 可能看不到最新数据。</summary>
		virtual void Unmap() = 0;

		virtual void UpdateData(const void* data, size_t size) = 0;
		/// <summary>获取缓冲字节大小。</summary>
		/// <returns>字节数（创建时由 RHIBufferDesc::SizeBytes 决定）。</returns>
		virtual uint32_t GetSize() const = 0;
	protected:
		RHIBuffer() = default;
	};

} // namespace DM::RHI
