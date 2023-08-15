#pragma once

#include "Kaesar/Renderer/Shader.h"
#include "Kaesar/Renderer/Texture.h"

#include <memory>

namespace Kaesar {
    class Material
    {
    public:
        Material() = default;
        Material(std::shared_ptr<Shader>& shader);

        void Bind();

        std::shared_ptr<Texture2D> GetTexture(const Sampler& sampler);
        std::unordered_map<uint32_t, std::shared_ptr<Texture2D>>& GetTextures() { return m_Textures; }

        std::shared_ptr<Shader> GetShader() { return m_Shader; }

        std::vector<PushConstant>& GetPushConstants() { return m_PushConstants; }
        std::vector<Sampler>& GetSamplers() { return m_Samplers; }

        static std::shared_ptr<Material> Create(std::shared_ptr<Shader>& shader);

    private:
        std::shared_ptr<Shader> m_Shader;

        std::unordered_map<uint32_t, std::shared_ptr<Texture2D>> m_Textures;

        std::vector<PushConstant> m_PushConstants;
        std::vector<Sampler> m_Samplers;
    };
}