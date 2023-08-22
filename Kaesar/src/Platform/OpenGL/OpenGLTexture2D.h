#pragma once

#include "Kaesar/Renderer/Texture.h"

#include <glad/glad.h>

namespace Kaesar {
    class OpenGLTexture2D : public Texture2D
    {
    public:
        OpenGLTexture2D(const std::string& filepath, bool vertical, bool sRGB, bool HDR);
        OpenGLTexture2D(uint32_t width, uint32_t height, const unsigned char* data, bool vertical, bool sRGB);

        virtual ~OpenGLTexture2D();

        virtual inline uint32_t GetWidth() const override { return m_Width; };
        virtual inline uint32_t GetHeight() const override { return m_Height; };
        virtual inline uint32_t GetRendererID() const override { return m_RendererID; };

        virtual void Bind(uint32_t slot) const override;
        virtual void UnBind() const override;

        virtual void SetData(void* data, uint32_t size) override;

        virtual bool operator==(const Texture& other) const override;

        virtual std::string GetPath() const override { return m_Filepath; }

        static void BindTexture(uint32_t rendererID, uint32_t slot);

    private:
        void LoadHDR();

    private:
        std::string m_Filepath;
        uint32_t m_Width, m_Height;
        uint32_t m_RendererID;
        GLenum m_InternalFormat, m_DataFormat; // m_InternalFormat 是纹理内部格式，m_DataFormat 是纹理数据格式
    };
}