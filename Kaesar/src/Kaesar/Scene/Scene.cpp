#include "krpch.h"
#include "Scene.h"

#include "Kaesar/Scene/Entity.h"
#include "Kaesar/Scene/Component.h"

namespace Kaesar {
    Scene::Scene() 
    {
        Entity::s_Scene = this;
        SceneRenderer::Initialize();
    }

    Scene::~Scene() {}

    Entity Scene::CreateEntity(const std::string& name) {
        Entity entity = { m_Registry.create() };
        m_Entities.push_back(std::make_shared<Entity>(entity));

        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name; // entity 名称
        entity.AddComponent<TransformComponent>(); // 所有 entity 都默认有 transform 组件

        return entity;
    }

    Entity Scene::DuplicateEntity(Entity entity)
    {
        Entity duplicateEntity = CreateEntity();
        if (duplicateEntity)
        {
            auto& entityTag = entity.GetComponent<TagComponent>();
            auto& entityTrans = entity.GetComponent<TransformComponent>();
            auto& duplicateEntityTag = duplicateEntity.GetComponent<TagComponent>();
            auto& duplicateEntityTrans = duplicateEntity.GetComponent<TransformComponent>();
            duplicateEntityTag.Tag = entityTag.Tag + u8"副本";
            duplicateEntityTrans = entityTrans;

            if (entity.HasComponent<CameraComponent>())
            {
                duplicateEntity.AddComponent<CameraComponent>(entity.GetComponent<CameraComponent>());
            }
            if (entity.HasComponent<MeshComponent>())
            {
                duplicateEntity.AddComponent<MeshComponent>(entity.GetComponent<MeshComponent>());
            }
        }

        return duplicateEntity;
    }

    entt::entity Scene::FindEntity(uint32_t id)
    {
        for (auto& e : m_Entities) {
            if (*e == (entt::entity)id) {
                return *e;
            }
        }
        return {};
    }

    void Scene::DestroyEntity(Entity entity) {
        m_Registry.destroy(entity); // 销毁 entity

        // 从 m_Entities 中移除 entity
        for (auto& e : m_Entities) {
            if (*e == entity) {
                auto it = std::find(m_Entities.begin(), m_Entities.end(), e);
                if (it != m_Entities.end()) {
                    m_Entities.erase(it);
                    break;
                }
            }
        }
    }

    void Scene::OnUpdateRuntime(Timestep ts) {
        //TODO: Implement OnUpdateRuntime
    }

    void Scene::OnUpdateEditor(Timestep ts, PerspectiveCamera& camera) {
        SceneRenderer::BeginScene(camera);
        SceneRenderer::RenderScene(*this);
        SceneRenderer::EndScene();
    }

    void Scene::OnViewportResize(uint32_t width, uint32_t height) {
        m_ViewportWidth = width;
        m_ViewportHeight = height;

        SceneRenderer::OnViewportResize(width, height);

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