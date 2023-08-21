#include "krpch.h"
#include "Model.h"

#include <glm/glm.hpp>
#include <filesystem>

namespace Kaesar {
    Model::Model(const std::string& path)
    {
        std::filesystem::path filepath(path);

        if (!filepath.is_absolute())
        {
            auto dir = std::filesystem::current_path();
            auto filePath = dir.string() + path;
            LoadModel(filePath);
        }
        else
        {
            LoadModel(path);
        }
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
        const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        m_Scene = scene;

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
        // 处理各个节点获取 Mesh
        for (auto i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(ProcessMesh(mesh, scene));
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
        std::vector<MeshTexture> meshTextures;

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

                // 处理顶点切线
                glm::vec3 tangent;
                tangent.x = mesh->mTangents[i].x;
                tangent.y = mesh->mTangents[i].y;
                tangent.z = mesh->mTangents[i].z;
                vertex.Tangent = tangent;

                glm::vec3 bitangent;
                bitangent.x = mesh->mBitangents[i].x;
                bitangent.y = mesh->mBitangents[i].y;
                bitangent.z = mesh->mBitangents[i].z;
                vertex.Bitangent = bitangent;
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
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // 1. 漫反射贴图
        std::vector<MeshTexture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        meshTextures.insert(meshTextures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. 高光贴图
        std::vector<MeshTexture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        meshTextures.insert(meshTextures.end(), specularMaps.begin(), specularMaps.end());
        // 3. 法线贴图
        std::vector<MeshTexture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        std::vector<MeshTexture> normalMaps2 = LoadMaterialTextures(material, aiTextureType_DISPLACEMENT, "texture_normal");
        meshTextures.insert(meshTextures.end(), normalMaps.begin(), normalMaps.end());
        meshTextures.insert(meshTextures.end(), normalMaps2.begin(), normalMaps2.end());
        // 4. 位移贴图
        std::vector<MeshTexture> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        meshTextures.insert(meshTextures.end(), heightMaps.begin(), heightMaps.end());

        return Mesh(vertices, indices, meshTextures);
    }

    std::vector<MeshTexture> Model::LoadMaterialTextures(aiMaterial* material, aiTextureType type, const std::string& typeName)
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
                std::shared_ptr<Texture2D> tex;
                if (m_Scene->GetEmbeddedTexture(str.C_Str())) // 如果是内嵌的纹理
                {
                    auto width = m_Scene->GetEmbeddedTexture(str.C_Str())->mWidth;
                    auto height = m_Scene->GetEmbeddedTexture(str.C_Str())->mHeight;

                    tex = Texture2D::Create(width, height,
                        reinterpret_cast<unsigned char*>(m_Scene->GetEmbeddedTexture(str.C_Str())->pcData), 
                        0, type == aiTextureType_DIFFUSE); // 内嵌纹理默认为漫反射贴图
                }
                else
                {
                    auto tex = Texture2D::Create(filename, 0, type == aiTextureType_DIFFUSE);
                }
                if (tex)
                {
                    textures.push_back(tex);
                    texture.id = tex->GetRendererID();
                    texture.type = typeName;
                    texture.path = str.C_Str();
                    tmpTextures.push_back(texture);
                    textures_loaded.push_back(texture); // add to loaded textures
                }
            }
        }

        return tmpTextures;
    }
}