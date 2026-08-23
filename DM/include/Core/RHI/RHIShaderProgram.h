#pragma once
#include"RHIResource.h"
namespace DM::RHI
{
	class RHIDescriptorSetGroup;
	class RHIDescriptorSet;
	class RHIShaderProgram:public RHIResource
	{
	public:
		RHIShaderProgram()=default;
		~RHIShaderProgram()=default;
		virtual EResourceType GetResourceType() const override { return EResourceType::ShaderProgram; }
		virtual RHIDescriptorSetGroup*			GenDescriptorSetGroup()const = 0;
		virtual std::vector<RHIDescriptorSet*>	GenDescriptorSets()const = 0;
		virtual RHIDescriptorSet*				GenDescriptorSet(uint32_t set)const = 0;
	private:

	};
}