#pragma once

#include "Kaesar/Renderer/Shader.h"
#include "Kaesar/Renderer/VertexArray.h"

#include <string>
#include <glm/glm.hpp>

namespace Kaesar {
    // Mesh 中一个顶点的结构
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    // Mesh 纹理形式的材质数据
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
        std::vector<Vertex> vertices; // Mesh 所有顶点
        std::vector<unsigned int> indices; //用于索引绘制的索引
        std::vector<MeshTexture> textures; // Mesh 所有材质数据

    private:
        std::shared_ptr<VertexArray> m_VertexArray;
    };
}