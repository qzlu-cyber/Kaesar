#pragma once

#include "RendererAPI.h"

#include "Kaesar/Renderer/Shader.h"
#include "Kaesar/Renderer/Model.h"

namespace Kaesar {
    class Renderer
    {
    public:
        static void BeginScene();
        static void EndScene();

        static void Submit(const std::shared_ptr<VertexArray>& vertexArray);
        static void Submit(const std::shared_ptr<Model>& model);
        static void Submit(const Model& model);

        static void OnWindowResize(uint32_t width, uint32_t height);

        static std::string RendererInfo();

        inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
    };
}