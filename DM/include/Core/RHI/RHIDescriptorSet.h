#pragma once
#include<Core/RHI/RHITypes.h>
namespace DM::RHI
{
	class RHIDescriptorSet
	{
	public:

		RHIDescriptorSet()= default;
		virtual ~RHIDescriptorSet()=default;
		//EDescriptorType GetType()const { return m_Type; }

		//virtual void BindUBO(RHIBuffer* ubo)const = 0;

		const std::vector<Descriptor>& GetDescriptors()const { return m_Descriptors; }

	protected:
		std::vector<Descriptor>m_Descriptors;
	};
}