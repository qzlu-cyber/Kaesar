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
        auto vertexBuffer = VertexBuffer::Create((float*)(&vertices[0]), vertices.size() * sizeof(Vertex));
        auto indexBuffer = IndexBuffer::Create(&indices[0], indices.size());
    
        m_VertexArray->Bind();

        BufferLayout layout = {
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float3, "a_Normal" },
            { ShaderDataType::Float2, "a_TexCoords" },
            { ShaderDataType::Float3,"a_Tangent" },
            { ShaderDataType::Float3,"a_Bitangent" }
        };

        vertexBuffer->SetLayout(layout);

        m_VertexArray->AddVertexBuffer(vertexBuffer);
        m_VertexArray->SetIndexBuffer(indexBuffer);

        m_VertexArray->Unbind();
    }
}