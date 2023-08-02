#pragma once

#include "Kaesar/Renderer/Mesh.h"
#include "Kaesar/Renderer/Shader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Kaesar {
	class Model
	{
	public:
		Model(const std::string& filepath);

		void Draw(const Shader& shader);

		~Model();

	private:
		void LoadModel(const std::string& filepath);
		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<MeshTexture> LoadMaterialtextures(aiMaterial* material, aiTextureType type, const std::string& typeName);

	public:
		std::vector<Mesh> m_Meshes;
		std::string m_Directory; // 模型所在目录

	};
}