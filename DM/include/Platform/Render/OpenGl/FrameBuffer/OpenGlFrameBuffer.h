#pragma once
#include<Core/Render/FrameBuffer.h>
namespace DM
{
	class DM_API OpenGlFrameBuffer:public FrameBuffer
	{
	public:
		OpenGlFrameBuffer(const FrameBufferSpecification& spec);
		virtual ~OpenGlFrameBuffer();
		virtual void Bind()override;
		virtual void UnBind()override;
		virtual int ReadPixel(const uint32_t& attachmentIndex, int x, int y)override;
		virtual void ClearColorAttachment(const uint32_t& attachmentIndex,  int data) override;
		virtual uint32_t GetColorAttachmentId(const uint32_t& index)const override { DM_CORE_ASSERT(m_ColorAttachmentIds.size() > index, "{}", "Out of range!"); return m_ColorAttachmentIds[index]; }
		virtual uint32_t GetDepthAttachmentId()const override { return m_DepthAttachmentId; }
		virtual void Resize(uint32_t width, uint32_t height)override;
		virtual uint32_t GetId()const override { return m_Id; };

	private:
		void FreeBuffer();
		void Reset();
	private:
		uint32_t m_Id;
		FrameBufferSpecification m_Spec;
		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecs;
		FramebufferTextureSpecification m_DepthAttachmentSpec;
		std::vector<uint32_t>m_ColorAttachmentIds;
		uint32_t m_DepthAttachmentId = 0;
	}; 
}


