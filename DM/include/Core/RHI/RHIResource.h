#pragma once
#include<cstdint>
#include<Core/RHI/RHITypes.h>
namespace DM::RHI
{
	class DM_API RHIResource
	{
	public:
		virtual ~RHIResource() = default;

		/// <summary>获取资源类型。</summary>
		/// <returns>资源类型枚举。</returns>
		virtual EResourceType GetResourceType() const=0;

	protected:
		RHIResource() = default;
	};

} // namespace DM::RHI
