#pragma once
#include"Core/RHI/RHIResource.h"
#include"Core/RHI/RHITypes.h"

namespace DM::RHI
{
	/// <summary>
	/// [笔记]渲染管线。opengl采用全局状态机的模式来管理状态，绑定着色程序的方式来执行渲染流程，而vulkan则将状态和着色器都打包成一个渲染管线
	/// </summary>
	class DM_API RHIPipeline : public RHIResource
	{
	public:
		virtual ~RHIPipeline() = default;

		EResourceType GetResourceType() const override { return EResourceType::Pipeline; }
	protected:
		RHIPipeline() = default;
	};

} // namespace DM::RHI
