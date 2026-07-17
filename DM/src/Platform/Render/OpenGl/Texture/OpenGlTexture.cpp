#include "DMPCH.h"
#include "Platform/Render/OpenGl/Texture/OpenGlTexture.h"
#include<glad/glad.h>
#include<stb_image.h>
#include<Tool/Util/Util.h>
#include<fstream>
namespace DM
{
    static uint32_t ConvertTextureDataFormatToByteSize(GLenum format)
    {
        switch (format)
        {
        case GL_RED:return 1;
        case GL_RGB:return 3;
        case GL_RGBA:return 4;
        }
        DM_CORE_ASSERT(false, "{}", "Unsuported texture format!");
        return 0;
    }
    OpenGlTexture2D::OpenGlTexture2D(const std::string_view& path)
    :m_Path(path.data())
    {
        stbi_set_flip_vertically_on_load(true);
        int width=0,height=0, channel = 0, desiredChannel=0;
		unsigned char* Data = stbi_load(path.data(), &width, &height, &channel, desiredChannel);
        DM_CORE_ASSERT(Data, "{}", "OpenGlTexture2D():Failed to load image");

        m_Width = width; m_Height = height;

        GLenum outDataFormat = 0;    //将数据写入纹理时的的格式
        GLenum internalFormat = 0;   //纹理内部存储格式
        desiredChannel = desiredChannel == 0 ? channel : desiredChannel;//Data的通道数
        if (desiredChannel == 1)
        {
            outDataFormat = GL_RED;
            internalFormat = GL_R8;  // 内部存储格式：单通道8位
        }
        else if (desiredChannel == 3)
        {
            outDataFormat = GL_RGB;
            internalFormat = GL_RGB8; // 三通道8位
        }
        else if (desiredChannel == 4)
        {
            outDataFormat = GL_RGBA;
            internalFormat = GL_RGBA8; // 四通道8位
        }
        else
        {
            DM_CORE_ASSERT(false, "{}", "OpenGlTexture2D():Unsupported desired channels");
        }
        m_InternalFormat = internalFormat;
        m_DataFormat = outDataFormat;
        glCreateTextures(GL_TEXTURE_2D, 1, &m_Id);
        glTextureStorage2D(m_Id, 1, internalFormat, width, height);
        glTextureParameteri(m_Id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_Id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(m_Id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_Id, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureSubImage2D(m_Id, 0, 0, 0, width, height, outDataFormat, GL_UNSIGNED_BYTE, Data);
        stbi_image_free(Data);
    }

    OpenGlTexture2D::OpenGlTexture2D(uint32_t width, uint32_t height)
    {
        m_Width = width;
        m_Height = height;
        GLenum outDataFormat = GL_RGBA;   
        GLenum internalFormat = GL_RGBA8;
        m_InternalFormat = internalFormat;
        m_DataFormat = outDataFormat;
        glCreateTextures(GL_TEXTURE_2D, 1, &m_Id);
        glTextureStorage2D(m_Id, 1, internalFormat, width, height);
        glTextureParameteri(m_Id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(m_Id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(m_Id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_Id, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureSubImage2D(m_Id, 0, 0, 0, width, height, outDataFormat, GL_UNSIGNED_BYTE, nullptr);
    }

    OpenGlTexture2D::~OpenGlTexture2D()
    {
        glDeleteTextures(1, &m_Id);
    }

    void OpenGlTexture2D::Bind(uint32_t unit)
    {
       /* glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, m_Id);*/
        glBindTextureUnit(unit, m_Id);
    }
    void OpenGlTexture2D::SetData(void* data, uint32_t size)
    {
        const uint32_t& channelSize = ConvertTextureDataFormatToByteSize(m_DataFormat);
        DM_CORE_ASSERT(size == m_Width * m_Height * channelSize, "{}", "Data must be entire texture!");
        glTextureSubImage2D(m_Id,0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
    }



}

