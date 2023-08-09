#include "krpch.h"
#include "OpenGLTexture2D.h"

#include "stb_image.h"

namespace Kaesar {
    OpenGLTexture2D::OpenGLTexture2D(const std::string& filepath, bool vertical)
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
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
        }
        else if (channels == 3)
        {
            internalFormat = GL_RGB8;
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

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLTexture2D::Active(int index = 0) const
    {
        glActiveTexture(GL_TEXTURE0 + index);
    }

    void OpenGLTexture2D::Bind() const
    {
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }

    void OpenGLTexture2D::Unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void OpenGLTexture2D::BindMultisample(unsigned int id) const
    {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, id);
    }

    void OpenGLTexture2D::UnbindMultisample() const
    {
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
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
}