#pragma once

#include "Kaesar/Scene/SceneCamera.h"
#include "Kaesar/Renderer/Model.h"
#include "Kaesar/Renderer/Shader.h"
#include "Kaesar/Renderer/Material.h"
#include "Kaesar/Renderer/Light.h"

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Kaesar {
    struct TagComponent
    {
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag)
            : Tag(tag) {}
    };

    struct TransformComponent
    {
        glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3& translation)
            : Translation(translation) {}

        glm::mat4 GetTransform() const
        {
            glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

            return glm::translate(glm::mat4(1.0f), Translation)
                * rotation
                * glm::scale(glm::mat4(1.0f), Scale);
        }
    };

    struct CameraComponent
    {
        SceneCamera Camera;
        bool FixedAspectRatio = false; // �̶���߱�
        bool Primary = true; // ��ʶ�������Ƿ�Ϊ�����
        // �����ͨ���ǳ����е���Ҫ��ͼ����������Ⱦ��������������,
        // ��һ�������������ĳ����У�����ͨ������һ�����Ϊ�������������һ��������û�����������Ҫ�ӽ�

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
    };

    struct MeshComponent 
    {
        Model model;
        std::string path; 

        MeshComponent() = default;
        MeshComponent(const MeshComponent&) = default;
        MeshComponent(const std::string& path)
            :path(path), model(path) {}
    };

    struct MaterialComponent
    {
        Material material;
        std::shared_ptr<Shader> shader;

        MaterialComponent() = default;

        MaterialComponent(const MaterialComponent&) = default;

        MaterialComponent(const std::shared_ptr<Material>& material, const std::shared_ptr<Shader>& shader)
            : material(*material), shader(shader)
        {}

        MaterialComponent(std::shared_ptr<Shader> shader) 
        {
            this->shader = shader;
            material = *Material::Create(shader);
        }
    };

    struct LightComponent
    {
        std::shared_ptr<Light> light;
        LightType type; // �ƹ�����

        LightComponent() 
        {
            type = LightType::Directional;
            light = std::make_shared<DirectionalLight>();
        };

        LightComponent(LightType otherType, std::shared_ptr<Light>& otherLight)
        {
            type = otherType;
            light = otherLight;
        }

        LightComponent(const LightComponent& other) = default;
    };
}
