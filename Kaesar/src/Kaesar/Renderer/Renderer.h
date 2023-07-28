#pragma once

#include "RendererAPI.h"

namespace Kaesar {
    class Renderer
    {
    public:
        static void BeginScene();
        static void EndScene();

        static void Submit(const std::shared_ptr<VertexArray>& vertexArray);

        static void OnWindowResize(uint32_t width, uint32_t height);

        inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
    };
}