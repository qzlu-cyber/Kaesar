#pragma once

#include "Kaesar/Renderer/Light.h"
#include "Kaesar/Renderer/UniformBuffer.h"

namespace Kaesar
{
    struct DirectionalLightData
    {
        glm::vec4 direction; // 平行光照射方向
        glm::vec4 color;
    };

    struct PointLightData
    {
        glm::vec4 position; // 点光源位置
        glm::vec4 color;
    };

    struct SpotLightData
    {
        glm::vec4 position; // 聚光灯位置
        glm::vec4 direction; // 聚光灯方向
        glm::vec4 color;
    };

    struct LightsParams
    {
        float pointLinear;
        float pointQuadratic;

        float spotLinear;
        float spotQuadratic;
        float innerCutOff;
        float outerCutOff;
    };

    class LightManager {

    public:
        LightManager(uint32_t binding);

        void IntitializeLights();
        void UpdateBuffer();

        void UpdateDirLight(DirectionalLight* dl, const glm::vec3& position);
        void UpdatePointLights(PointLight* pl, const glm::vec3& position, uint32_t index);
        void UpdateSpotLights(SpotLight* sl, const glm::vec3& position, uint32_t index);

        ~LightManager() = default;

    private:
        std::shared_ptr<UniformBuffer> m_LightsBuffer, m_LightsParamsBuffer;

        DirectionalLightData m_DirLight;
        PointLightData m_PointLights[5];
        SpotLightData m_SpotLights[5];
        LightsParams m_LightsParams[5];
    };
}