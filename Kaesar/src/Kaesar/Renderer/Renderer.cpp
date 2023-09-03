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

    void Renderer::Submit(Material& material, const Model& model)
    {
        material.Bind();
        auto& meshes = model.meshes;
        for (auto& mesh : meshes) 
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
            if (mesh.textures.size() == 0) {
                Texture2D::BindTexture(0, 0);
            }
            unsigned int diffuseNr = 1;
            unsigned int specularNr = 1;
            unsigned int normalNr = 1;
            unsigned int heightNr = 1;
            for (unsigned int i = 0; i < mesh.textures.size(); i++)
            {
                std::string number;
                std::string name = mesh.textures[i].type;
                if (name == "texture_diffuse")
                    Texture2D::BindTexture(mesh.textures[i].id, 0);
                else if (name == "texture_specular")
                    Texture2D::BindTexture(mesh.textures[i].id, 1);
                else if (name == "texture_normal")
                    Texture2D::BindTexture(mesh.textures[i].id, 2);
                //else if (name == "texture_height")
                //	number = std::to_string(heightNr++); // transfer unsigned int to stream
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