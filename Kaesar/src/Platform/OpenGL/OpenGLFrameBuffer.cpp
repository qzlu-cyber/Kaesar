#include "krpch.h"
#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>

namespace Kaesar {
    Kaesar::OpenGLFrameBuffer::OpenGLFrameBuffer(const FramebufferSpecification& fspc)
        : m_Specification(fspc)
    {
        glGenFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

        // 生成纹理附件
        glGenTextures(1, &m_ColorAttachment);
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);
        // 将其绑定到当前帧缓冲对象
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

        // 生成渲染缓冲对象附件
        glGenRenderbuffers(1, &m_DepthAttachment);
        glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        // 将其绑定到当前帧缓冲对象
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthAttachment);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    OpenGLFrameBuffer::~OpenGLFrameBuffer()
    {
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures(1, &m_ColorAttachment);
        glDeleteRenderbuffers(1, &m_DepthAttachment);
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
}
