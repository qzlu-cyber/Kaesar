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
    /// �����ʰ󶨵���Ⱦ��ˮ���У���������ɫ�����������Դ����׼��������Ⱦ
    /// </summary>
    void Material::Bind()
    {
        m_Shader->Bind();

        // ������������ȡ��������
        for (auto& sampler : m_Samplers)
        {
            if (sampler.isUsed)
            {
                auto& texture = m_Textures[sampler.binding]; // ���ݲ������İ󶨵�Ӳ��ʵ��������л�ȡ��Ӧ���������
                if (texture) 
                {
                    texture->Bind(sampler.binding); // ������󶨵���Ⱦ��ˮ����
                }
            }
        }
    }

    std::shared_ptr<Texture2D> Material::GetTexture(const Sampler& sampler)
    {
        return  m_Textures[sampler.binding];
    }
}