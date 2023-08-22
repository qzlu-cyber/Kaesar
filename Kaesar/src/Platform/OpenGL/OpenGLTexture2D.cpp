#include "krpch.h"
#include "OpenGLTexture2D.h"

#include "stb_image.h"

namespace Kaesar {
    OpenGLTexture2D::OpenGLTexture2D(const std::string& filepath, bool vertical, bool sRGB, bool HDR)
        : m_Filepath(filepath)
    {
        if (HDR)
        {
            LoadHDR();
        }
        else
        {
            glGenTextures(1, &m_RendererID);
            glBindTexture(GL_TEXTURE_2D, m_RendererID);

            int width, height, channels;
            stbi_set_flip_vertically_on_load(vertical);
            unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
            KR_CORE_ASSERT(data, "纹理 {0} 加载失败！", filepath);

            m_Width = width;
            m_Height = height;

            GLenum internalFormat = 0, dataFormat = 0;
            if (channels == 4)
            {
                internalFormat = sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8;
                dataFormat = GL_RGBA;
            }
            else if (channels == 3)
            {
                internalFormat = sRGB ? GL_SRGB8 : GL_RGB8;
                dataFormat = GL_RGB;
            }

            m_InternalFormat = internalFormat;
            m_DataFormat = dataFormat;

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(data);
        }
    }

    OpenGLTexture2D::OpenGLTexture2D(uint32_t mWidth, uint32_t mHeight, const unsigned char* data, bool vertical, bool sRGB)
    {
        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        int width, height, channels;
        unsigned char* mdata = nullptr;
        if (mHeight == 0)
        {
            mdata = stbi_load_from_memory(data, mWidth, &width, &height, &channels, 0);
        }
        else
        {
            mdata = stbi_load_from_memory(data, mWidth * mHeight, &width, &height, &channels, 0);
        }

        m_Width = width;
        m_Height = height;

        GLenum internalFormat = 0, dataFormat = 0;
        if (channels == 4)
        {
            internalFormat = sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8;
            dataFormat = GL_RGBA;
        }
        else if (channels == 3)
        {
            internalFormat = sRGB ? GL_SRGB8 : GL_RGB8;
            dataFormat = GL_RGB;
        }

        m_InternalFormat = internalFormat;
        m_DataFormat = dataFormat;

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(mdata);
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLTexture2D::Bind(uint32_t slot) const
    {
        glBindTextureUnit(slot, m_RendererID);
    }

    void OpenGLTexture2D::UnBind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void OpenGLTexture2D::SetData(void* data, uint32_t size)
    {
        uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
        KR_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
    }

    bool OpenGLTexture2D::operator==(const Texture& other) const
    {
        // 使用 dynamic_cast 确保转换的正确性
        const OpenGLTexture2D* otherOpenGLTexture = dynamic_cast<const OpenGLTexture2D*>(&other);
        if (otherOpenGLTexture)
            return this->GetRendererID() == otherOpenGLTexture->GetRendererID();

        // 如果转换失败，返回 false
        return false;
    }

    void OpenGLTexture2D::BindTexture(uint32_t rendererID, uint32_t slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTextureUnit(slot, rendererID);
    }

    void OpenGLTexture2D::LoadHDR()
    {
        int width, height, channels;
        float* data = stbi_loadf(m_Filepath.c_str(), &width, &height, &channels, 0);
        
        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
}