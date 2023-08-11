#pragma once

#include <glm/glm.hpp>

namespace Kaesar {
    enum class LightType
    {
        Directional,
        Point,
        Spot
    };

    class Light
    {
    public:
        Light() = default;
        Light(const glm::vec3& color)
            : m_Color(color)
        {}

        glm::vec3& GetPosition() { return m_Position; }
        void SetPosition(const glm::vec3& position) { m_Position = position; }

        glm::vec3& GetAmbient() { return m_Ambient; }
        void SetAmbient(const glm::vec3& ambient) { m_Ambient = ambient; }

        glm::vec3& GetDiffuse() { return m_Diffuse; }
        void SetDiffuse(const glm::vec3& diffuse) { m_Diffuse = diffuse; }

        glm::vec3& GetSpecular() { return m_Specular; }
        void SetSpecular(const glm::vec3& specular) { m_Specular = specular; }

        glm::vec3 GetColor() const { return m_Color; }
        void SetColor(const glm::vec3& color) { m_Color = color; }

        virtual ~Light() = default;

    private:
        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 m_Ambient  = { 1.0f, 1.0f, 1.0f };
        glm::vec3 m_Diffuse  = { 1.0f, 1.0f, 1.0f };
        glm::vec3 m_Specular = { 1.0f, 1.0f, 1.0f };

        glm::vec3 m_Color = { 1.0f, 1.0f, 1.0f };
    };

    class DirectionalLight : public Light
    {
    public:
        DirectionalLight() = default;
        DirectionalLight(const glm::vec3& direction)
            : m_Direction(direction)
        {}

        const glm::vec3& GetDirection() const { return m_Direction; }
        void SetDirection(const glm::vec3& direction) { m_Direction = direction; }

        ~DirectionalLight() = default;

    private:
        glm::vec3 m_Direction = { 10.0f, 10.0f, 10.0f };
    };

    class PointLight : public Light
    {
    public:
        PointLight(const glm::vec3& color)
            : Light(color)
        {}

        float GetConstant() const { return m_Constant; }
        void SetConstant(float constant) { m_Constant = constant; }

        float GetLinear() const { return m_Linear; }
        void SetLinear(float linear) { this->m_Linear = linear; }

        float GetQuadratic() const { return m_Quadratic; }
        void SetQuadratic(float quadratic) { m_Quadratic = quadratic; }

        ~PointLight() = default;

    private:
        float m_Constant = 1.0f; // 常数项
        float m_Linear = 0.0f; // 一次衰减项
        float m_Quadratic = 0.0f; // 二次衰减项
    };

    class SpotLight : public Light
    {
    public:
        SpotLight(const glm::vec3& color) 
            : Light(color)
        {}
        SpotLight(const glm::vec3& position, const glm::vec3& spotDirection)
            : Light(position), m_SpotDirection(spotDirection)
        {}

        const glm::vec3& GetSpotDirection() const { return m_SpotDirection; }
        void SetSpotDirection(const glm::vec3& spotDirection) { m_SpotDirection = spotDirection; }

        float GetInnerCutOff() const { return m_CutOff; }
        void SetInnerCutOff(float cutOff) { m_CutOff = cutOff; }

        float GetOuterCutOff() const { return m_OuterCutOff; }
        void SetOuterCutOff(float outerCutOff) { m_OuterCutOff = outerCutOff; }

        ~SpotLight() = default;

    private:
        glm::vec3 m_SpotDirection = { 1.0f, 1.0f, 1.0f };
        float m_CutOff = 0.0f; // 切光角
        float m_OuterCutOff = 0.0f; // 外切光角
    };
}