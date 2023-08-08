#include "krpch.h"
#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>

namespace Kaesar {
    static const uint32_t s_MaxFramebufferSize = 8192;

    /// <summary>
    /// 生成的纹理目标
    /// </summary>
    /// <param name="multisampled">是否是多重采样</param>
    /// <returns>纹理目标</returns>
    static GLenum TextureTarget(bool multisampled)
    {
        return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    }

    /// <summary>
    /// 生成纹理附件
    /// </summary>
    /// <param name="multisampled">是否是多重采样</param>
    /// <param name="count">生成的纹理附件个数</param>
    /// <param name="ID">存储生成的纹理对象名称的数组</param>
    static void CreateTextures(bool multisampled, uint32_t count, uint32_t* ID)
    {
        for (auto i = 0; i < count; i++)
        {
            glGenTextures(1, &ID[i]);
        }
    }

    static void BindTexture(bool multisampled, uint32_t id)
    {
        glBindTexture(TextureTarget(multisampled), id);
    }

    /// <summary>
    /// 生成渲染缓冲对象附件
    /// </summary>
    /// <param name="ID">存储生成的纹理对象</param>
    static void CreateRenderBuffer(uint32_t ID)
    {
        glGenRenderbuffers(1, &ID);
        glBindRenderbuffer(GL_RENDERBUFFER, ID);
    }

    /// <summary>
    /// 给帧缓冲对象附加纹理附件
    /// </summary>
    /// <param name="colorAttachmentID">要附加的纹理本身</param>
    /// <param name="samples">采样数</param>
    /// <param name="internalFormat">纹理内部格式</param>
    /// <param name="format">纹理格式</param>
    /// <param name="width">纹理宽度</param>
    /// <param name="height">纹理高度</param>
    /// <param name="index">颜色附件索引</param>
    static void AttachColorTexture(uint32_t colorAttachmentID, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
    {
        bool multisampled = samples > 1;
        if (samples > 1)
        {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        // 解绑纹理附件
        //glBindTexture(TextureTarget(multisampled), 0);

        // 将其绑定到当前帧缓冲对象
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), colorAttachmentID, 0);
    }

    //? 不知道为什么不能用渲染缓冲添加深度和模板附件，但是用纹理附件就可以 😅
    /// <summary>
    /// 为帧缓冲对象附加渲染缓冲附件（用于深度和模板测试）
    /// </summary>
    /// <param name="renderBufferID">渲染缓冲对象名称</param>
    /// <param name="samples">采样数</param>
    /// <param name="internalFormat">渲染缓冲内部格式</param>
    /// <param name="width">附件宽度</param>
    /// <param name="height">附件高度</param>
    static void AttachRenderBuffer(uint32_t renderBufferID, int samples, GLenum internalFormat, uint32_t width, uint32_t height)
    {
        if (samples > 1)
        {
            // 创建多重采样渲染缓冲附件
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internalFormat, width, height);
        }
        else
        {
            // 创建普通渲染缓冲附件
            glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
        }

        // 解绑渲染缓冲对象附件
        //glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // 将渲染缓冲对象附件绑定到当前帧缓冲对象的深度和模板附件
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferID);
    }

    static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
    {
        bool multisampled = samples > 1;
        if (multisampled)
        {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
        }
        else
        {
            glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
    }

    static GLenum TextureFormatToGL(FramebufferTextureFormat format)
    {
        switch (format)
        {
            case FramebufferTextureFormat::RGBA8:       return GL_RGBA8;
            case FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
        }

        KR_CORE_ASSERT(false, "未定义帧缓冲附件格式！");

        return 0;
    }

    static bool IsDepthFormat(FramebufferTextureFormat format)
    {
        switch (format)
        {
            case FramebufferTextureFormat::DEPTH24STENCIL8:  return true;
        }

        return false;
    }

    Kaesar::OpenGLFrameBuffer::OpenGLFrameBuffer(const FramebufferSpecification& fspc)
        : m_Specification(fspc)
    {
        for (auto& format : m_Specification.Attachments.Attachments)
        {
            if (!IsDepthFormat(format.TextureFormat)) {
                m_ColorAttachmentSpecifications.emplace_back(format);
            }
            else
            {
                m_DepthAttachmentSpecification = format;
            }
        }

        Invalidate();
    }

    OpenGLFrameBuffer::~OpenGLFrameBuffer()
    {
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
        glDeleteRenderbuffers(1, &m_DepthAttachment);
    }

    void OpenGLFrameBuffer::Invalidate()
    {
        if (m_RendererID)
        {
            glDeleteFramebuffers(1, &m_RendererID);
            glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
            glDeleteTextures(1, &m_DepthAttachment);

            m_ColorAttachments.clear();
            m_DepthAttachment = 0;
        }

        glGenFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

        bool multisample = m_Specification.Samples > 1;
        
        // 处理颜色附件
        if (m_ColorAttachmentSpecifications.size())
        {
            m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
            CreateTextures(multisample, m_ColorAttachments.size(), m_ColorAttachments.data());

            for (size_t i = 0; i < m_ColorAttachments.size(); i++) {
                BindTexture(multisample, m_ColorAttachments[i]);

                switch (m_ColorAttachmentSpecifications[i].TextureFormat)
                {
                case FramebufferTextureFormat::RGBA8:
                    AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGBA8, GL_RGBA, m_Specification.Width, m_Specification.Height, i);
                    break;
                case FramebufferTextureFormat::RED_INTEGER:
                    AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_R32I, GL_RED_INTEGER, m_Specification.Width, m_Specification.Height, i);
                    break;
                }
            }
        }

        // 处理深度附件
        if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
        {
            CreateTextures(multisample, 1, &m_DepthAttachment);
            BindTexture(multisample, m_DepthAttachment);
            switch (m_DepthAttachmentSpecification.TextureFormat)
            {
            case FramebufferTextureFormat::DEPTH24STENCIL8:
                //AttachRenderBuffer(m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
                AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, 
                    GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, 
                          m_Specification.Width, m_Specification.Height);
                break;
            }
        }

        if (m_ColorAttachments.size() > 1)
        {
            KR_CORE_ASSERT(m_ColorAttachments.size() <= 4, "Kaesar只支持附加 4 个颜色附件！");

            GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
            
            glDrawBuffers(m_ColorAttachments.size(), buffers); // 指定帧缓冲的颜色附件
        }
        else if (m_ColorAttachments.empty())
        {
            // Only depth-pass
            glDrawBuffer(GL_NONE);
        }

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            KR_CORE_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLFrameBuffer::Bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glViewport(0, 0, m_Specification.Width, m_Specification.Height);
    }

    void OpenGLFrameBuffer::Unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLFrameBuffer::BlitMultiSample(unsigned int readFrameBuffer, unsigned int drawFrameBuffer) const
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, readFrameBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFrameBuffer);
        glBlitFramebuffer(0, 0, m_Specification.Width, m_Specification.Height, 0, 0, m_Specification.Width, m_Specification.Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
        {
            KR_CORE_ERROR("Attempted to rezize framebuffer to {0}, {1}", width, height);
            return;
        }
        m_Specification.Width = width;
        m_Specification.Height = height;
        Invalidate();
    }

    int OpenGLFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
    {
        KR_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "索引必须小于帧缓冲颜色附件的个数！");

        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
        int pixelData;
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);

        return pixelData;
    }

    void OpenGLFrameBuffer::ClearAttachment(uint32_t attachmentIndex, int value)
    {
        KR_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "索引必须小于帧缓冲颜色附件的个数！");

        auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
        glClearTexImage(m_ColorAttachments[attachmentIndex], 0,
            TextureFormatToGL(spec.TextureFormat), GL_INT, &value);
    }
}
