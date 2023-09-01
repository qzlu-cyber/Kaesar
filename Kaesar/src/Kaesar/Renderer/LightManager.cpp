#include "krpch.h"
#include "LightManager.h"

namespace Kaesar {
	LightManager::LightManager(uint32_t binding)
	{
		m_LightsBuffer = UniformBuffer::Create(sizeof(m_PointLights) + sizeof(m_SpotLights) + sizeof(m_DirLight), binding);
		m_LightsParamsBuffer = UniformBuffer::Create(sizeof(m_LightsParamsBuffer), binding + 1);
	}

	void LightManager::IntitializeLights()
	{
        m_DirLight.direction = glm::vec4(0.0f);
        m_DirLight.color = glm::vec4(0.0f);

        for (auto& pointLight : m_PointLights)
        {
            pointLight.position = glm::vec4(0.0f);
            pointLight.color = glm::vec4(0.0f);
        }

        for (auto& spotLight : m_SpotLights)
        {
            spotLight.position = glm::vec4(0.0f);
            spotLight.direction = glm::vec4(0.0f);
            spotLight.color = glm::vec4(0.0f);
        }

        for (auto& lightParams : m_LightsParams)
        {
            lightParams.pointLinear = 0.09f;
            lightParams.pointQuadratic = 0.032f;
            lightParams.spotLinear = 0.09f;
            lightParams.spotQuadratic = 0.032f;
            lightParams.innerCutOff = glm::cos(glm::radians(12.5f));
            lightParams.outerCutOff = glm::cos(glm::radians(15.0f));
        }
	}

    void LightManager::UpdateBuffer()
    {
        m_LightsBuffer->SetData(&m_PointLights, sizeof(m_PointLights), 0);
        m_LightsBuffer->SetData(&m_SpotLights, sizeof(m_SpotLights), sizeof(m_PointLights));
        m_LightsBuffer->SetData(&m_DirLight, sizeof(m_DirLight), sizeof(m_PointLights) + sizeof(m_SpotLights));
        m_LightsParamsBuffer->SetData(&m_LightsParams, sizeof(m_LightsParams), 0);
    }

    void LightManager::UpdateDirLight(DirectionalLight* dl, const glm::vec3& position)
    {
        m_DirLight.color = glm::vec4(dl->GetColor(), 0) * dl->GetIntensity();
        m_DirLight.direction = glm::vec4(dl->GetDirection(), 0);
    }

    void LightManager::UpdatePointLights(PointLight* pl, const glm::vec3& position, uint32_t index)
    {
        m_PointLights[index].color = glm::vec4(pl->GetColor(), 1) * pl->GetIntensity();
        m_PointLights[index].position = glm::vec4(position, 1);

        m_LightsParams[index].pointLinear = pl->GetLinear();
        m_LightsParams[index].pointQuadratic = pl->GetQuadratic();
    }

    void LightManager::UpdateSpotLights(SpotLight* sl, const glm::vec3& position, uint32_t index)
    {
        m_SpotLights[index].color = glm::vec4(sl->GetColor(), 1) * sl->GetIntensity();
        m_SpotLights[index].position = glm::vec4(position, 1);
        m_SpotLights[index].direction = glm::vec4(sl->GetDirection(), 0);

        m_LightsParams[index].spotLinear = sl->GetLinear();
        m_LightsParams[index].spotQuadratic = sl->GetQuadratic();
        m_LightsParams[index].innerCutOff = glm::cos(glm::radians(sl->GetInnerCutOff()));
        m_LightsParams[index].outerCutOff = glm::cos(glm::radians(sl->GetOuterCutOff()));
    }
}