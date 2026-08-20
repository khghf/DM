#pragma once
#include"RHITypes.h"
namespace DM::RHI
{
	class RHIDescriptorSetGroup
	{
	public:
		RHIDescriptorSetGroup()=default;
		~RHIDescriptorSetGroup()=default;

		virtual void BindUBO(RHIBuffer* ubo, const std::string& name)const = 0;
		virtual void BindTex(RHITexture* tex, const std::string& name)const = 0;

	private:

	};
}