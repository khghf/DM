#pragma once
#include<Core/Render/Texture/Texture.h>
namespace DM
{
	class DM_API OpenGlTexture2D:public Texture2D
	{
	public:
		OpenGlTexture2D(const std::string_view& path);
		OpenGlTexture2D(uint32_t width,uint32_t height);
		~OpenGlTexture2D();
		virtual void Bind(uint32_t unit=0) override;
		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual void SetData(void* data, uint32_t size) override;
		virtual bool operator==(const Texture& other)const override { return m_Id == ((const OpenGlTexture2D&)other).m_Id; }
		virtual uint32_t GetId() { return m_Id; };
	private:
		/*virtual void	SaveToLocal(void*data,size_t size)override;
		virtual void* ReadFromLocal()override;*/
	private:
		uint32_t m_Width, m_Height;
		uint32_t m_Id;
		std::string m_Path;
		unsigned int m_DataFormat;
		unsigned int m_InternalFormat;
	};
}
