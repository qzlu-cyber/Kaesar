#include "krpch.h"
#include "Renderer.h"
#include "RenderCommand.h"

namespace Kaesar {
    void Renderer::BeginScene()
    {

    }

    void Renderer::EndScene()
    {

    }

    void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray)
    {
        vertexArray->Bind();
        RenderCommand::DrawIndexed(vertexArray);
    }

    void Renderer::Submit(const std::shared_ptr<Model>& model)
    {
        auto meshs = model->m_Meshes;
        for (const auto& mesh : meshs)
        {
            auto vertexArray = mesh.GetVertexArray();
            vertexArray->Bind();
            RenderCommand::DrawIndexed(vertexArray);
        }
    }

    void Renderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        RenderCommand::SetViewport(0, 0, width, height);
    }

    std::string Renderer::RendererInfo()
    {
        return RenderCommand::Info();
    }
}