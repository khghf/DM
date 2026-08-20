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
		virtual EResourceType GetAssetType() const override { return EResourceType::ShaderProgram; }
		virtual std::vector<RHIDescriptorSet*>	GenDescriptorSets()const = 0;
		virtual RHIDescriptorSetGroup*			GenDescriptorSetGroup()const = 0;
	private:

	};
}