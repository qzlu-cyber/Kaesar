#include "krpch.h"
#include "Model.h"

#include <glm/glm.hpp>
#include <filesystem>

namespace Kaesar {
    Model::Model(const std::string& path)
    {
        auto dir = std::filesystem::current_path();
        auto filepath = dir.string() + path;

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

        directory = filepath.substr(0, filepath.find_last_of('\\'));

        ProcessNode(scene->mRootNode, scene);
    }

    void Model::ProcessNode(aiNode* node, const aiScene* scene)
    {
        // ��������ڵ��ȡ Mesh
        for (auto i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(ProcessMesh(mesh, scene));
        }

        // �ݹ��ӽڵ�
        for (auto i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene);
        }
    }

    Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<MeshTexture> meshTextures;

        // ������
        for (auto i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;

            // ������λ��
            glm::vec3 vector;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            // �����㷨��
            if (mesh->HasNormals())
            {
                glm::vec3 normal;
                normal.x = mesh->mNormals[i].x;
                normal.y = mesh->mNormals[i].y;
                normal.z = mesh->mNormals[i].z;
                vertex.Normal = normal;
            }

            // ��������������
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

        // ��������
        for (auto i = 0; i < mesh->mNumFaces; i++) // ÿ��������ͼԪ
        {
            aiFace face = mesh->mFaces[i];
            for (auto i = 0; i < face.mNumIndices; i++) // ÿ��ͼԪ������
            {
                indices.push_back(face.mIndices[i]);
            }
        }

        // �������
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // 1. ��������ͼ
        std::vector<MeshTexture> diffuseMaps = LoadMaterialtextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        meshTextures.insert(meshTextures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. �߹���ͼ
        std::vector<MeshTexture> specularMaps = LoadMaterialtextures(material, aiTextureType_SPECULAR, "texture_specular");
        meshTextures.insert(meshTextures.end(), specularMaps.begin(), specularMaps.end());
        // 3. ������ͼ
        std::vector<MeshTexture> normalMaps = LoadMaterialtextures(material, aiTextureType_HEIGHT, "texture_normal");
        meshTextures.insert(meshTextures.end(), normalMaps.begin(), normalMaps.end());
        // 4. λ����ͼ
        std::vector<MeshTexture> heightMaps = LoadMaterialtextures(material, aiTextureType_AMBIENT, "texture_height");
        meshTextures.insert(meshTextures.end(), heightMaps.begin(), heightMaps.end());

        return Mesh(vertices, indices, meshTextures);
    }

    std::vector<MeshTexture> Model::LoadMaterialtextures(aiMaterial* material, aiTextureType type, const std::string& typeName)
    {
        std::vector<MeshTexture> tmpTextures;
        for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
        {
            aiString str;
            material->GetTexture(type, i, &str);
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    tmpTextures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }

            if (!skip)
            {   
                MeshTexture texture;
                std::string filename = str.C_Str();
                filename = directory + '\\' + filename;
                auto tex = Texture2D::Create(filename, 0);
                textures.push_back(tex);
                texture.id = tex->GetRendererID();
                texture.type = typeName;
                texture.path = str.C_Str();
                tmpTextures.push_back(texture);
                textures_loaded.push_back(texture); // add to loaded textures
            }
        }

        return tmpTextures;
    }
}