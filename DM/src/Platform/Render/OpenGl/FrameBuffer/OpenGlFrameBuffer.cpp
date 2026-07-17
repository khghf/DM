#include "DMPCH.h"
#include "Platform/Render/OpenGl/FrameBuffer/OpenGlFrameBuffer.h"
#include<Core/Render/Renderer/Renderer.h>
#include<glad/glad.h>
namespace DM
{
	static uint32_t s_MaxFrameBufferSize = 8192;
	static bool		IsDepthFormat(FramebufferTextureFormat format)
	{
		switch (format)
		{
		case DM::FramebufferTextureFormat::None:			return false;
		case DM::FramebufferTextureFormat::RGBA8:			return false;
		case DM::FramebufferTextureFormat::DEPTH24_STENCIL8:return true;
		}
		return false;
	}
	static GLenum	TextureTarget(bool bMultiSample) { return bMultiSample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D; }
	static void		BindTexture(bool bMultiSample, uint32_t id) { glBindTexture(TextureTarget(bMultiSample), id); }
	static void		CreateTextures(bool bMultiSample, uint32_t* outId, uint32_t count)
	{
		glCreateTextures(TextureTarget(bMultiSample), count, outId);
	}
	static void		AttachColorAttachment(const uint32_t&id,const uint32_t&sample,const GLenum&internalFormat, const GLenum& readFormat,const uint32_t&width,const uint32_t&height,const uint32_t&index)
	{
		bool bMultiSample = sample > 1;
		if (bMultiSample)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sample, internalFormat, width, height, GL_FALSE);
		}
		else
		{
			glTextureStorage2D(id, 1, internalFormat, width, height);
			glTextureParameteri(id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+index, TextureTarget(bMultiSample), id, 0);
	}
	static void		AttachDepthAttachment(const uint32_t& id, const uint32_t& sample, const GLenum& internalFormat,const GLenum&attachmentType, const uint32_t& width, const uint32_t& height)
	{
		bool bMultiSample = sample > 1;
		if (bMultiSample)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sample, internalFormat, width, height, GL_FALSE);
		}
		else
		{
			glTextureStorage2D(id, 1, internalFormat, width, height);
			glTextureParameteri(id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(bMultiSample), id, 0);
	}
	static GLenum   TextureForamtToGL(const FramebufferTextureFormat& format)
	{
		switch (format)
		{
			case DM::FramebufferTextureFormat::RED_Int:return GL_RED_INTEGER;
			case DM::FramebufferTextureFormat::RGBA8:return GL_RGBA8;
			case DM::FramebufferTextureFormat::DEPTH24_STENCIL8:return GL_DEPTH24_STENCIL8;
		}
		DM_CORE_ASSERT(false, "{}", "invalid texture format!");
		return GL_NONE;
	}

	OpenGlFrameBuffer::OpenGlFrameBuffer(const FrameBufferSpecification& spec) :
		m_Spec(spec), m_Id(0)
	{
		for (const auto& format : m_Spec.Attachments.TexAttachments)
		{
			if (!IsDepthFormat(format.TextureFormat))
			{
				m_ColorAttachmentSpecs.emplace_back(format);
			}
			else
			{
				m_DepthAttachmentSpec = format;
			}
		}
		
		Reset();
	}

	OpenGlFrameBuffer::~OpenGlFrameBuffer()
	{
		FreeBuffer();
	}
	void OpenGlFrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
	}
	void OpenGlFrameBuffer::UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	int OpenGlFrameBuffer::ReadPixel(const uint32_t& attachmentIndex, int x, int y)
	{
		//if ((uint32_t)x<m_Spec.Width || (uint32_t)y>m_Spec.Height)return -10;
		DM_CORE_ASSERT(attachmentIndex<m_ColorAttachmentIds.size(),"attachmentIndex:{} is out of range!","attachmentIndex");
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int val = -1;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &val);
		return val;
	}

	void OpenGlFrameBuffer::ClearColorAttachment(const uint32_t& attachmentIndex,  int data)
	{
		DM_CORE_ASSERT(attachmentIndex < m_ColorAttachmentIds.size(), "attachmentIndex:{} is out of range!", "attachmentIndex");
		auto& spec = m_ColorAttachmentSpecs[attachmentIndex];
		glClearTexImage(m_ColorAttachmentIds[attachmentIndex], 0, TextureForamtToGL(spec.TextureFormat), GL_INT, &data);
	}

	void OpenGlFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width <= 0 || height <= 0 || width > s_MaxFrameBufferSize || height > s_MaxFrameBufferSize)
		{
			LOG_CORE_ERROR("Attemp to resize framebuffer to[width:{},height:{}]", width, height);
			return;
		}
		m_Spec.Width = width;
		m_Spec.Height = height;
		Reset();
	}

	void OpenGlFrameBuffer::FreeBuffer()
	{
		if (m_Id)
		{
			glDeleteFramebuffers(1, &m_Id);
			glDeleteTextures((GLsizei)m_ColorAttachmentIds.size(), m_ColorAttachmentIds.data());
			glDeleteTextures(1, &m_DepthAttachmentId);
			m_ColorAttachmentIds.clear();
			m_DepthAttachmentId = 0;
		}
	}

	void OpenGlFrameBuffer::Reset()
	{
		FreeBuffer();
		glCreateFramebuffers(1, &m_Id);
		glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
		bool bMultiSample = m_Spec.Smaples > 1 ? true : false;
		if (m_ColorAttachmentSpecs.size())
		{
			m_ColorAttachmentIds.resize(m_ColorAttachmentSpecs.size(),0);
			CreateTextures(bMultiSample, m_ColorAttachmentIds.data(), (uint32_t)m_ColorAttachmentIds.size());
			for (int i=0;i<m_ColorAttachmentIds.size();++i)
			{
				const auto& id = m_ColorAttachmentIds[i];
				const auto& texFormat = m_ColorAttachmentSpecs[i].TextureFormat;
				BindTexture(m_Spec.Smaples > 1 ? true : false, id);
				switch (texFormat)
				{
					case DM::FramebufferTextureFormat::RGBA8:AttachColorAttachment(id, m_Spec.Smaples, GL_RGBA8, GL_RGBA, m_Spec.Width, m_Spec.Height, i); break;
					case DM::FramebufferTextureFormat::RED_Int:AttachColorAttachment(id, m_Spec.Smaples, GL_R32I, GL_RED_INTEGER, m_Spec.Width, m_Spec.Height, i); break;
				}
			}
		}
		if (m_DepthAttachmentSpec.TextureFormat != FramebufferTextureFormat::None)
		{
			CreateTextures(bMultiSample, &m_DepthAttachmentId, 1);
			BindTexture(bMultiSample, m_DepthAttachmentId);
			switch (m_DepthAttachmentSpec.TextureFormat)
			{
				case DM::FramebufferTextureFormat::DEPTH24_STENCIL8:AttachDepthAttachment(m_DepthAttachmentId, m_Spec.Smaples, GL_DEPTH24_STENCIL8, GL_DEPTH_ATTACHMENT, m_Spec.Width, m_Spec.Height); break;
			}
		}
		if (m_ColorAttachmentIds.size() > 1)
		{
			DM_CORE_ASSERT(m_ColorAttachmentIds.size() <= 4,"{}","FrameBuffer only supports 4 color attachemnts!");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3 };
			glDrawBuffers((GLsizei)m_ColorAttachmentIds.size(), buffers);
		}
		else if(m_ColorAttachmentIds.empty())
		{
			glDrawBuffer(GL_NONE);
		}
		DM_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER)== GL_FRAMEBUFFER_COMPLETE, "{}", "Framebuffer is incompleted!");
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

}