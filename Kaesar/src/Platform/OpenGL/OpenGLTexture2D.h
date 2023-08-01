#pragma once

#include "Kaesar/Renderer/Texture.h"

#include <glad/glad.h>

namespace Kaesar {
    class OpenGLTexture2D : public Texture2D
    {
    public:
        OpenGLTexture2D(const std::string& filepath);

        virtual ~OpenGLTexture2D();

        virtual inline uint32_t GetWidth() const override { return m_Width; };
        virtual inline uint32_t GetHeight() const override { return m_Height; };
        virtual inline uint32_t GetRendererID() const override { return m_RendererID; };

        virtual void Active(int index) const override;
        virtual void Bind(uint32_t slot = 0) const override;

        virtual bool operator==(const Texture& other) const override;

    private:
        std::string m_Filepath;
        uint32_t m_Width, m_Height;
        uint32_t m_RendererID;
        GLenum m_InternalFormat, m_DataFormat; // m_InternalFormat 是纹理内部格式，m_DataFormat 是纹理数据格式
    };
}