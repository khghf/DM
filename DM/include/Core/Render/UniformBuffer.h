#pragma once
#include <Core/MMM/Reference.h>
namespace DM
{
	class DM_API UniformBuffer
	{
	public:
		virtual ~UniformBuffer() {}
		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

		static SPtr<UniformBuffer> Create(uint32_t size, uint32_t binding);
	};
}


