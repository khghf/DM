#pragma once
#include<Core/MMM/Reference.h>
#include<Core/Core.h>
namespace DM
{
	enum class FramebufferTextureFormat
	{
		None = 0,
		RED_Int,
		RGBA8,
		DEPTH24_STENCIL8,
		Depth = DEPTH24_STENCIL8,
	};

	struct DM_API FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format) :TextureFormat(format)
		{
		}
		FramebufferTextureFormat TextureFormat= FramebufferTextureFormat::None;
	};

	struct DM_API FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification>attachments):TexAttachments(attachments)
		{

		}
		std::vector<FramebufferTextureSpecification> TexAttachments;
	};

	struct DM_API FrameBufferSpecification
	{
		uint32_t Width, Height;
		uint32_t Smaples = 1;
		FramebufferAttachmentSpecification Attachments;
	};
	class DM_API FrameBuffer
	{
	public:
		virtual ~FrameBuffer()=default;
		virtual void Bind() = 0;
		virtual void UnBind() = 0;
		virtual int ReadPixel(const uint32_t& attachmentIndex, int x, int y) = 0;
		virtual void ClearColorAttachment(const uint32_t& attachmentIndex,int data) = 0;
		virtual uint32_t GetColorAttachmentId(const uint32_t& index=0)const = 0;
		virtual uint32_t GetDepthAttachmentId()const = 0;
		virtual void Resize(uint32_t width,uint32_t height) = 0;
		virtual uint32_t GetId()const = 0;
		static SPtr<FrameBuffer>Create(const FrameBufferSpecification& spec);
	};
	
}


