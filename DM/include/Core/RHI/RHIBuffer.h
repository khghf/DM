#pragma once

#include"Core/RHI/RHIResource.h"
#include"Core/RHI/RHITypes.h"

namespace DM::RHI
{
	
	class DM_API RHIBuffer : public RHIResource
	{
	public:
		virtual ~RHIBuffer() = default;

		
		virtual void* Map() = 0;

		virtual void Unmap() = 0;

		virtual void Update(const void* data, size_t size) = 0;
		
		virtual uint32_t GetSize() const = 0;


	protected:
		RHIBuffer() = default;
	};

} // namespace DM::RHI
