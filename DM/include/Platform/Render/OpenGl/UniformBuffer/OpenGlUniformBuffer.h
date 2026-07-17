#pragma once
#include"Core/Render/UniformBuffer.h"
namespace DM
{
	class DM_API OpenGlUniformBuffer:public UniformBuffer
	{
	public:
		OpenGlUniformBuffer(uint32_t size, uint32_t binding);
		virtual ~OpenGlUniformBuffer();

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
	private:
		uint32_t m_Id = 0;
	};
}


