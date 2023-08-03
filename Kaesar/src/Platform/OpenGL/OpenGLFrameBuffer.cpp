#include "krpch.h"
#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>

namespace Kaesar {
    static const uint32_t s_MaxFramebufferSize = 8192;

    Kaesar::OpenGLFrameBuffer::OpenGLFrameBuffer(const FramebufferSpecification& fspc)
        : m_Specification(fspc)
    {
        Invalidate();
    }

    OpenGLFrameBuffer::~OpenGLFrameBuffer()
    {
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures(1, &m_ColorAttachment);
        glDeleteRenderbuffers(1, &m_DepthAttachment);
    }

    void OpenGLFrameBuffer::Invalidate()
    {
        if (m_RendererID)
        {
            glDeleteFramebuffers(1, &m_RendererID);
            glDeleteTextures(1, &m_ColorAttachment);
            glDeleteRenderbuffers(1, &m_DepthAttachment);
        }

        glGenFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

        // ������ MSAA
        if (m_Specification.Samples == 1)
        {
            // ����������
            glGenTextures(1, &m_ColorAttachment);
            glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glBindTexture(GL_TEXTURE_2D, 0);
            // ����󶨵���ǰ֡�������
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

            // ������Ⱦ������󸽼�
            glGenRenderbuffers(1, &m_DepthAttachment);
            glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            // ����󶨵���ǰ֡�������
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthAttachment);
        }
        else
        {
            // ����������
            glGenTextures(1, &m_ColorAttachment);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Specification.Samples, GL_RGB, m_Specification.Width, m_Specification.Height, GL_TRUE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
            // ����󶨵���ǰ֡�������
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment, 0);

            // ������Ⱦ������󸽼�
            glGenRenderbuffers(1, &m_DepthAttachment);
            glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_Specification.Samples, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            // ����󶨵���ǰ֡�������
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthAttachment);
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
}
