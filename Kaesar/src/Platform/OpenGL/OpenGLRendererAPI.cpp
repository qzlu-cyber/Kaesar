#include "krpch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Kaesar {
    GLenum RenderStateToGLState(RenderState state)
    {
        switch (state)
        {
            case Kaesar::RenderState::DEPTH_TEST: return GL_DEPTH_TEST;
            case Kaesar::RenderState::BLEND:      return GL_BLEND;
            case Kaesar::RenderState::CULL:       return GL_CULL_FACE;
            case Kaesar::RenderState::SRGB:       return GL_FRAMEBUFFER_SRGB;
        }
        KR_CORE_ASSERT(false, "ÉèÖÃäÖÈ¾×´Ì¬Ê§°Ü£¬Ã»ÓÐ´Ë×´Ì¬£¡");

        return 0;
    }

    void OpenGLRendererAPI::Init()
    {
        glFrontFace(GL_CW);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        //glCullFace(GL_FRONT);
        //glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
    }

    std::string OpenGLRendererAPI::GetRendererInfo()
    {
        std::string info;
        info += u8"Çý¶¯: " + std::string((char*)glGetString(GL_VENDOR)) + "\n";
        info += u8"äÖÈ¾Æ÷: " + std::string((char*)glGetString(GL_RENDERER)) + "\n";
        info += u8"°æ±¾: " + std::string((char*)glGetString(GL_VERSION));
        return info;
    }

    void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void OpenGLRendererAPI::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRendererAPI::ClearColor()
    {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void OpenGLRendererAPI::EnableDepthTest()
    {
        glEnable(GL_DEPTH_TEST);
    }

    void OpenGLRendererAPI::DisableDepthTest()
    {
        glDisable(GL_DEPTH_TEST);
    }

    void OpenGLRendererAPI::SetState(RenderState state, bool on)
    {
        on ? glEnable(RenderStateToGLState(state)) : glDisable(RenderStateToGLState(state));
    }

    void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
    {
        glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
    }

    void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        glViewport(x, y, width, height);
    }
}