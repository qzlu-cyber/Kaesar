#pragma once

#include "RendererAPI.h"

namespace Kaesar {
    class RenderCommand
    {
    public:
        inline static void Init() { s_RendererAPI->Init(); }
        inline static std::string Info() { return s_RendererAPI->GetRendererInfo(); }
        inline static void SetClearColor(const glm::vec4& color) { s_RendererAPI->SetClearColor(color); }
        inline static void Clear() { s_RendererAPI->Clear(); }
        inline static void ClearColor() { s_RendererAPI->ClearColor(); }
        inline static void EnableDepthTest() { s_RendererAPI->EnableDepthTest(); }
        inline static void DisableDepthTest() { s_RendererAPI->DisableDepthTest(); }
        inline static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
        {
            s_RendererAPI->DrawIndexed(vertexArray);
        }
        inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
        {
            s_RendererAPI->SetViewport(x, y, width, height);
        }

    private:
        static RendererAPI* s_RendererAPI;
    };
}