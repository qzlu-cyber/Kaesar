#pragma once

#include "Kaesar/Renderer/Shader.h"
#include "Kaesar/Renderer/Texture.h"

#include <memory>

namespace Kaesar {
    class Material
    {
    public:
        struct ShaderMaterial
        {
            glm::vec4 color = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
            float MetallicFactor = 0;
            float RoughnessFactor = 1;
            float AO = 1;
        };

        struct CBuffer
        {
            ShaderMaterial material;
            int id;
            float tiling;
            int HasAlbedoMap;
            int HasNormalMap;
            int HasMetallicMap;
            int HasRoughnessMap;
            int HasAOMap;

            CBuffer()
                : id(-1), tiling(1), HasAOMap(0), HasNormalMap(0), HasRoughnessMap(0), HasAlbedoMap(0) {}
        };

    public:
        Material() = default;
        Material(const Material& material);
        Material(std::shared_ptr<Shader>& shader);

        void Bind();

        std::shared_ptr<Texture2D> GetTexture(const Sampler& sampler);
        std::unordered_map<uint32_t, std::shared_ptr<Texture2D>>& GetTextures() { return m_Textures; }
        void SetTextures(const std::unordered_map<uint32_t, std::shared_ptr<Texture2D>>& textures) { m_Textures = textures; }

        std::shared_ptr<Shader> GetShader() { return m_Shader; }

        std::vector<PushConstant>& GetPushConstants() { return m_PushConstants; }
        std::vector<Sampler>& GetSamplers() { return m_Samplers; }

        CBuffer GetCBuffer() const { return m_Cbuffer; }

        static std::shared_ptr<Material> Create(std::shared_ptr<Shader>& shader);

        void Set(const std::string& name, int value);
        void Set(const std::string& name, float value);
        void Set(const std::string& name, const glm::vec3& value);
        void Set(const std::string& name, const glm::vec4& value);

    private:
        std::shared_ptr<Shader> m_Shader;
        CBuffer m_Cbuffer;

        std::unordered_map<uint32_t, std::shared_ptr<Texture2D>> m_Textures;

        std::vector<PushConstant> m_PushConstants;
        std::vector<Sampler> m_Samplers;
    };
}