#pragma once

#include "Kaesar/Renderer/Shader.h"
#include "Kaesar/Renderer/VertexArray.h"

#include <string>
#include <glm/glm.hpp>

namespace Kaesar {
    // Mesh ��һ������Ľṹ
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    // Mesh ������ʽ�Ĳ�������
    struct MeshTexture
    {
        unsigned int id;
        std::string type;
    };

    class Mesh
    {
    public:
        Mesh(
            const std::vector<Vertex>& vertices, 
            const std::vector<unsigned int>& indices, 
            const std::vector<MeshTexture>& textures
        );

        ~Mesh();

        inline std::shared_ptr<VertexArray> GetVertexArray() const { return m_VertexArray; }

    private:
        void SetupMesh();
    
    public:
        std::vector<Vertex> vertices; // Mesh ���ж���
        std::vector<unsigned int> indices; //�����������Ƶ�����
        std::vector<MeshTexture> textures; // Mesh ���в�������

    private:
        std::shared_ptr<VertexArray> m_VertexArray;
    };
}