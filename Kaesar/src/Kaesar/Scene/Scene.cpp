#include "krpch.h"
#include "Scene.h"

#include "Kaesar/Scene/Entity.h"
#include "Kaesar/Scene/Component.h"

namespace Kaesar {
    Scene::Scene() {}
    Scene::~Scene() {}

    Entity Scene::CreateEntity(const std::string& name) {
        std::shared_ptr<Entity> entity = std::make_shared<Entity>(m_Registry.create(), this);
        m_Entities.push_back(entity);

        auto& tag = (*entity).AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name; // entity 名称
        (*entity).AddComponent<TransformComponent>(); // 所有 entity 都默认有 transform 组件

        return *entity;
    }

    void Scene::DestroyEntity(Entity entity) {
        m_Registry.destroy(entity);
    }

    void Scene::OnUpdateRuntime(Timestep ts) {
        //TODO: Implement OnUpdateRuntime
    }

    void Scene::OnUpdateEditor(Timestep ts, std::shared_ptr<PerspectiveCamera>& camera) {
        auto group = m_Registry.group<TransformComponent>(entt::get<TagComponent>);
        for (auto& entity : group)
        {
            auto& tag = m_Registry.get<TagComponent>(entity);
            
        }
    }

    void Scene::OnViewportResize(uint32_t width, uint32_t height) {
        m_ViewportWidth = width;
        m_ViewportHeight = height;

        auto view = m_Registry.view<CameraComponent>();
        for (auto& entity : view)
        {
            auto& cameraComponent = m_Registry.get<CameraComponent>(entity);
            if (!cameraComponent.FixedAspectRatio)
            {
                cameraComponent.Camera.SetViewportSize(width, height);
            }
        }
    }
}