#pragma once
#include"Core/RHI/RHIResource.h"
#include"Core/RHI/RHITypes.h"

namespace DM::RHI
{
	class DM_API RHIPipeline : public RHIResource
	{
	public:
		virtual ~RHIPipeline() = default;

		/// <summary>资源类型为管线（覆写基类）。</summary>
		EResourceType GetAssetType() const override { return EResourceType::Pipeline; }
	protected:
		RHIPipeline() = default;
	};

} // namespace DM::RHI
