#include "krpch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Kaesar {
    void OpenGLRendererAPI::Init()
    {
        glEnable(GL_MULTISAMPLE); // 开启多重采样
    }

    std::string OpenGLRendererAPI::GetRendererInfo()
    {
        std::string info;
        info += u8"驱动: " + std::string((char*)glGetString(GL_VENDOR)) + "\n";
        info += u8"渲染器: " + std::string((char*)glGetString(GL_RENDERER)) + "\n";
        info += u8"版本: " + std::string((char*)glGetString(GL_VERSION));
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

    void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
    {
        glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
    }

    void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        glViewport(x, y, width, height);
    }
}