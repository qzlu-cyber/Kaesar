#include "krpch.h"
#include "Mesh.h"

#include "Kaesar/Renderer/Texture.h"

namespace Kaesar {
    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<MeshTexture>& textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        SetupMesh();
    }

    Mesh::~Mesh()
    {
    }

    void Mesh::SetupMesh()
    {
        m_VertexArray = VertexArray::Create();
        auto vertexBuffer = VertexBuffer::Create((float*)(&vertices[0]), sizeof(vertices));
        auto indexBuffer = IndexBuffer::Create(&indices[0], sizeof(indices) / sizeof(uint32_t));
    
        m_VertexArray->Bind();

        BufferLayout layout = {
            { ShaderDataType::Float3, "a_Position"},
            { ShaderDataType::Float3, "a_Normal"},
            { ShaderDataType::Float3, "a_TexCoords"}
        };

        vertexBuffer->SetLayout(layout);

        m_VertexArray->AddVertexBuffer(vertexBuffer);
        m_VertexArray->SetIndexBuffer(indexBuffer);

        m_VertexArray->Unbind();
    }
}