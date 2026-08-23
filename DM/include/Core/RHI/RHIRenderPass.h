#pragma once
#include"RHIResource.h"
namespace DM::RHI
{
	class RHIFramebuffer;

	/// <summary>
	/// [笔记]渲染通道，vulkan中用于控制渲染数据到帧缓冲的详细过程包括：渲染附件的指定(颜色、深度、模板)，渲染过程中附件的布局(渲染前的、渲染后的)，
	/// 在渲染前后对附件的操作(前：是否清除旧数据、后：结束通道后是否保留数据)，总之RenderPass用于描述如何将数据渲染到帧缓冲中，而帧缓冲则提供
	/// 对应的内存资源、还有渲染通道与帧缓冲的某些设置要匹配。
	/// 
	/// 为了离屏渲染我没有将帧缓冲当作渲染通道的成员而是抽离了出来
	/// </summary>
	class DM_API RHIRenderPass:public RHIResource
	{
	public:
		RHIRenderPass()=default;
		virtual ~RHIRenderPass()=default;
		virtual EResourceType GetResourceType() const override { return EResourceType::RenderPass; };

		virtual RHIFramebuffer* CreateFramebuffer(const RHIFramebufferDesc& desc) = 0;
	private:

	};

}