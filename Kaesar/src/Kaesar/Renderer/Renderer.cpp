#include "krpch.h"
#include "Renderer.h"

#include "Kaesar/Renderer/RenderCommand.h"
#include "Kaesar/Renderer/Texture.h"

namespace Kaesar {
    void Renderer::BeginScene()
    {

    }

    void Renderer::EndScene()
    {

    }

    void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader)
    {
        vertexArray->Bind();
        RenderCommand::DrawIndexed(vertexArray);
    }

    void Renderer::Submit(const std::shared_ptr<Model>& model)
    {
        auto meshs = model->meshes;
        for (const auto& mesh : meshs)
        {
            auto vertexArray = mesh.GetVertexArray();
            vertexArray->Bind();
            RenderCommand::DrawIndexed(vertexArray);
        }
    }

    void Renderer::Submit(const Model& model, const std::shared_ptr<Shader>& shader)
    {
        auto meshs = model.meshes;

        for (const auto& mesh : meshs)
        {
            // bind appropriate textures
            unsigned int diffuseNr = 1;
            unsigned int specularNr = 1;
            unsigned int normalNr = 1;
            unsigned int heightNr = 1;
            for (unsigned int i = 0; i < mesh.textures.size(); i++)
            {
                Texture2D::BindTexture(mesh.textures[i].id, i);

                std::string number;
                std::string name = mesh.textures[i].type;
                if (name == "texture_diffuse")
                    number = std::to_string(diffuseNr++);
                else if (name == "texture_specular")
                    number = std::to_string(specularNr++);
                else if (name == "texture_normal")
                    number = std::to_string(normalNr++);
                else if (name == "texture_height")
                    number = std::to_string(heightNr++);

                shader->SetInt(name + number, i);
            }

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