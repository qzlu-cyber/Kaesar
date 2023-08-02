#include "krpch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Kaesar {
    void OpenGLRendererAPI::Init()
    {
        const char* vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        const char* renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
        const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        KR_CORE_WARN("Driver: {0}", vendor);
        KR_CORE_WARN("Renderer: {0}", renderer);
        KR_CORE_WARN("Version: {0}", version);
        glEnable(GL_MULTISAMPLE); // 开启多重采样
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

    void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
    {
        glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
    }

    void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        glViewport(x, y, width, height);
    }
}