#include "krpch.h"
#include "Material.h"

namespace Kaesar {
    Material::Material(std::shared_ptr<Shader>& shader)
    {
        m_Shader = shader;
        m_Samplers = shader->GetSamplers();
        m_PushConstants = shader->GetPushConstants();
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
            if (sampler.isUsed)
            {
                auto& texture = m_Textures[sampler.binding]; // 根据采样器的绑定点从材质的纹理集合中获取相应的纹理对象
                if (texture) 
                {
                    if (sampler.binding == 0)
                    {
                        m_Shader->SetInt("pc.HasDiffuseMap", 1);
                    }
                    if (sampler.binding == 2)
                    {
                        m_Shader->SetInt("pc.HasNormalMap", 1);
                    }
                    texture->Bind(sampler.binding); // 将纹理绑定到渲染流水线中
                }
            }
            else
            {
                if (sampler.binding == 0)
                {
                    m_Shader->SetInt("push.HasDiffuseMap", 0);
                }
                if (sampler.binding == 2)
                {
                    m_Shader->SetInt("push.HasNormalMap", 0);
                }
            }
        }
    }

    std::shared_ptr<Texture2D> Material::GetTexture(const Sampler& sampler)
    {
        return  m_Textures[sampler.binding];
    }
}