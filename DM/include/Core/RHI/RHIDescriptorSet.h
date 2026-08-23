#pragma once
#include<Core/RHI/RHITypes.h>
namespace DM::RHI
{
	/// <summary>
	/// [笔记]描述符集。描述符集是vulkan着色器访问资源的桥梁，vulkan支持在着色器代码中使用layout(set=(0,1,2....),binding=0)来声明变量，
	/// 每一个set就代表着一个描述符集，需要访问资源时只需要绑定描述符集即可，绑定后其下的所有binding的绑定都会随着一起改变，前提是
	/// 该描述符集绑定好了资源。
	/// 
	/// </summary>
	class RHIDescriptorSet
	{
	public:

		RHIDescriptorSet()= default;
		virtual ~RHIDescriptorSet()=default;


		const std::vector<Descriptor>& GetDescriptors()const { return m_Descriptors; }
		virtual void BindUBO(RHIBuffer* ubo, const std::string& name)const =0;
		virtual void BindTex(RHITexture* tex, const std::string& name)const =0;
	protected:
		std::vector<Descriptor>m_Descriptors;
	};
}