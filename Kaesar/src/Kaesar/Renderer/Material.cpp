#include "krpch.h"
#include "Material.h"

namespace Kaesar {
    Material::Material(std::shared_ptr<Shader>& shader)
    {
        m_Shader = shader;
        m_Samplers = shader->GetSamplers();
        m_PushConstants = shader->GetPushConstants();
    }

    Material::Material(const Material& material)
    {
        m_Shader = material.m_Shader;
        m_Samplers = m_Shader->GetSamplers();
        m_PushConstants = m_Shader->GetPushConstants();
        m_Textures = material.m_Textures;
        m_Cbuffer = material.m_Cbuffer;
    }

    std::shared_ptr<Material> Material::Create(std::shared_ptr<Shader>& shader)
    {
        return std::make_shared<Material>(shader);
    }

    /// <summary>
    /// 将材质绑定到渲染流水线中，包括绑定着色器和纹理等资源，以准备进行渲染
    /// </summary>
    void Material::Bind()
    {
        m_Shader->Bind();

        // 遍历采样器获取纹理数据
        for (auto& sampler : m_Samplers)
        {
            auto& texture = m_Textures[sampler.binding]; // 根据采样器的绑定点从材质的纹理集合中获取相应的纹理对象
            if (sampler.isUsed && texture)
            {
                if (sampler.binding == 0)
                    m_Shader->SetInt("pc.HasAlbedoMap", 1);
                if (sampler.binding == 1)
                    m_Shader->SetInt("pc.HasMetallicMap", 1);
                if (sampler.binding == 2)
                    m_Shader->SetInt("pc.HasNormalMap", 1);
                if (sampler.binding == 3)
                    m_Shader->SetInt("pc.HasRoughnessMap", 1);
                if (sampler.binding == 4)
                    m_Shader->SetInt("pc.HasAOMap", 1);

                texture->Bind(sampler.binding);
            }
            else
            {
                if (sampler.binding == 0)
                {
                    m_Shader->SetInt("pc.HasAlbedoMap", 0);
                }
                if (sampler.binding == 1)
                {
                    m_Shader->SetInt("pc.HasMetallicMap", 0);
                }
                if (sampler.binding == 2)
                {
                    m_Shader->SetInt("pc.HasNormalMap", 0);
                }
                if (sampler.binding == 3)
                {
                    m_Shader->SetInt("pc.HasRoughnessMap", 0);
                }
                if (sampler.binding == 4)
                {
                    m_Shader->SetInt("pc.HasAOMap", 0);
                }
            }
        }

        // 设置 PushConstants
        for (auto& item : m_PushConstants[1].members)
        {
            if (item.name == "material")
            {
                m_Shader->SetFloat4("pc.material.color", m_Cbuffer.material.color);
                m_Shader->SetFloat("pc.material.MetallicFactor", m_Cbuffer.material.MetallicFactor);
                m_Shader->SetFloat("pc.material.RoughnessFactor", m_Cbuffer.material.RoughnessFactor);
                m_Shader->SetFloat("pc.material.AO", m_Cbuffer.material.AO);
                m_Shader->SetFloat("pc.tiling", m_Cbuffer.tiling);
            }
        }
    }

    std::shared_ptr<Texture2D> Material::GetTexture(const Sampler& sampler)
    {
        return  m_Textures[sampler.binding];
    }

    void Material::Set(const std::string& name, int value)
    {
        if (name == "HasAlbedoMap")
        {
            m_Cbuffer.HasAlbedoMap = value;
        }
        else if (name == "HasNormalMap")
        {
            m_Cbuffer.HasNormalMap = value;
        }
        else if (name == "HasMetallicMap")
        {
            m_Cbuffer.HasMetallicMap = value;
        }
        else if (name == "HasRoughnessMap")
        {
            m_Cbuffer.HasRoughnessMap = value;
        }
        else if (name == "HasAOMap")
        {
            m_Cbuffer.HasAOMap = value;
        }
        else if (name == "tiling")
        {
            m_Cbuffer.tiling = value;
        }
    }

    void Material::Set(const std::string& name, float value)
    {
        if (name == "pc.material.MetallicFactor")
        {
            m_Cbuffer.material.MetallicFactor = value;
        }
        else if (name == "pc.material.RoughnessFactor")
        {
            m_Cbuffer.material.RoughnessFactor = value;
        }
        else if (name == "pc.material.AO")
        {
            m_Cbuffer.material.AO = value;
        }
    }

    void Material::Set(const std::string& name, const glm::vec3& value)
    {

    }

    void Material::Set(const std::string& name, const glm::vec4& value)
    {
        if (name == "pc.material.color")
        {
            m_Cbuffer.material.color = value;
        }
    }
}