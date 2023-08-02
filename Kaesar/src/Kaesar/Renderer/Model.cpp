#include "krpch.h"
#include "Model.h"

#include <glm/glm.hpp>

namespace Kaesar {
    Model::Model(const std::string& filepath)
    {
        LoadModel(filepath);
    }

    void Model::Draw(const Shader& shader)
    {
    }

    Model::~Model()
    {
    }

    void Model::LoadModel(const std::string& filepath)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);

        if ((!scene) || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || (!scene->mRootNode))
        {
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }

        m_Directory = filepath.substr(0, filepath.find_last_of('/'));

        ProcessNode(scene->mRootNode, scene);
    }

    void Model::ProcessNode(aiNode* node, const aiScene* scene)
    {
        // 处理各个节点获取 Mesh
        for (auto i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            m_Meshes.push_back(ProcessMesh(mesh, scene));
        }

        // 递归子节点
        for (auto i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene);
        }
    }

    Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<MeshTexture> textures;

        // 处理顶点
        for (auto i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;

            // 处理顶点位置
            glm::vec3 vector;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            // 处理顶点法线
            if (mesh->HasNormals())
            {
                glm::vec3 normal;
                normal.x = mesh->mNormals[i].x;
                normal.y = mesh->mNormals[i].y;
                normal.z = mesh->mNormals[i].z;
                vertex.Normal = normal;
            }

            // 处理顶点纹理坐标
            if (mesh->mTextureCoords[0])
            {
                glm::vec2 texCoords;
                texCoords.x = mesh->mTextureCoords[0][i].x;
                texCoords.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = texCoords;
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            vertices.push_back(vertex);
        }

        // 处理索引
        for (auto i = 0; i < mesh->mNumFaces; i++) // 每个三角形图元
        {
            aiFace face = mesh->mFaces[i];
            for (auto i = 0; i < face.mNumIndices; i++) // 每个图元的索引
            {
                indices.push_back(face.mIndices[i]);
            }
        }

        // 处理材质
        aiMaterial* material = scene->mMaterials[scene->mNumMaterials];

        return Mesh(vertices, indices, textures);
    }

    std::vector<MeshTexture> Model::LoadMaterialtextures(aiMaterial* material, aiTextureType type, const std::string& typeName)
    {
        return std::vector<MeshTexture>();
    }


}