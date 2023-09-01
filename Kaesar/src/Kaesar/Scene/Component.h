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
        bool FixedAspectRatio = false; // 固定宽高比
        bool Primary = true; // 标识相机组件是否为主相机
        // 主相机通常是场景中的主要视图，它负责渲染整个场景的内容,
        // 在一个包含多个相机的场景中，可以通过设置一个相机为主相机来决定哪一个相机是用户所看到的主要视角

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
        LightType type; // 灯光类型

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
