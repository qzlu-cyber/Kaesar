#include "krpch.h"
#include "Scene.h"

#include "Kaesar/Scene/Entity.h"
#include "Kaesar/Scene/Component.h"

namespace Kaesar {
    Scene::Scene(const std::string& name)
        : m_Name(name)
    {
        Entity::s_Scene = this;
        SceneRenderer::Initialize();

        m_Camera = new PerspectiveCamera(45.0f, 1.778f, 0.1f, 100.0f);
    }

    Scene::~Scene() 
    {
        delete m_Camera;
    }

    Entity Scene::CreateEntity(const std::string& name) 
    {
        Entity entity = { m_Registry.create() };
        m_Entities.push_back(std::make_shared<Entity>(entity));

        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" + std::to_string(uint32_t(entity)) : name; // entity 名称
        entity.AddComponent<TransformComponent>(); // 所有 entity 都默认有 transform 组件

        return entity;
    }

    std::shared_ptr<Entity> Scene::CreateEntity(Entity other)
    {
        Entity entity = { m_Registry.create() };
        std::shared_ptr<Entity> ent = std::make_shared<Entity>(entity);

        ent->AddComponent<TagComponent>(other.GetComponent<TagComponent>().Tag + u8"副本");
        ent->AddComponent<TransformComponent>(other.GetComponent<TransformComponent>());

        if (other.HasComponent<CameraComponent>()) 
        {
            ent->AddComponent<CameraComponent>(other.GetComponent<CameraComponent>());
        }
        if (other.HasComponent<MeshComponent>()) 
        {
            ent->AddComponent<MeshComponent>(other.GetComponent<MeshComponent>());
        }
        if (other.HasComponent<MaterialComponent>())
        {
            ent->AddComponent<MaterialComponent>(other.GetComponent<MaterialComponent>());
        }
        if (other.HasComponent<LightComponent>())
        {
            ent->AddComponent<LightComponent>(other.GetComponent<LightComponent>());
        }

        m_Entities.push_back(ent);

        return ent;
    }

    entt::entity Scene::FindEntity(uint32_t id)
    {
        for (auto& e : m_Entities) 
        {
            if (*e == (entt::entity)id) 
            {
                return *e;
            }
        }
        return {};
    }

    void Scene::DestroyEntity(Entity entity) 
    {
        m_Registry.destroy(entity); // 销毁 entity

        // 从 m_Entities 中移除 entity
        for (auto& e : m_Entities) 
        {
            if (*e == entity) 
            {
                auto it = std::find(m_Entities.begin(), m_Entities.end(), e);
                if (it != m_Entities.end()) {
                    m_Entities.erase(it);
                    break;
                }
            }
        }
    }

    std::shared_ptr<Entity> Scene::CreatePrimitive(PrimitiveType type)
    {
        auto entity = this->CreateEntity();

        std::string path;

        switch (type)
        {
            case PrimitiveType::Cube:
            {
                entity.GetComponent<TagComponent>().Tag = " 立方体";
                path = std::string("\\assets\\models\\cube\\cube.obj");
                entity.AddComponent<MeshComponent>(path);
                break;
            }
            case PrimitiveType::Plane:
            {
                entity.GetComponent<TagComponent>().Tag = " 平面";
                path = std::string("\\assets\\models\\plane\\plane.obj");
                entity.AddComponent<MeshComponent>(path);
                break;
            }
            case PrimitiveType::Sphere:
            {
                entity.GetComponent<TagComponent>().Tag = " 球体";
                path = std::string("\\assets\\models\\sphere\\sphere.fbx");
                entity.AddComponent<MeshComponent>(path);
                break;
            }
            default:
                break;
        }

        return std::make_shared<Entity>(entity);
    }

    std::shared_ptr<Entity> Scene::CreateLight(LightType type)
    {
        auto entity = this->CreateEntity();
        std::string path;
        std::shared_ptr<Light> light;
        switch (type)
        {
            case LightType::Directional:
            {
                entity.GetComponent<TagComponent>().Tag = " 平行光";
                light = std::make_shared<DirectionalLight>(glm::vec3(1.0f));
                entity.AddComponent<LightComponent>(type, light);
                break;
            }
            case LightType::Point:
            {
                entity.GetComponent<TagComponent>().Tag = " 点光源";
                light = std::make_shared<PointLight>(glm::vec3(1.0f));
                entity.AddComponent<LightComponent>(type, light);
                break;
            }
            case LightType::Spot:
            {
                entity.GetComponent<TagComponent>().Tag = " 聚光";
                light = std::make_shared<SpotLight>(glm::vec3(1.0f));
                entity.AddComponent<LightComponent>(type, light);
                break;
            }
            default:
                break;
        }

        return std::make_shared<Entity>(entity);
    }

    void Scene::OnUpdateRuntime(Timestep ts) 
    {
        //TODO: Implement OnUpdateRuntime
    }

    void Scene::OnUpdateEditor(Timestep ts)
    {
        SceneRenderer::BeginScene(*m_Camera);
        SceneRenderer::RenderScene();
        SceneRenderer::EndScene();
    }

    void Scene::OnViewportResize(uint32_t width, uint32_t height) 
    {
        m_ViewportWidth = width;
        m_ViewportHeight = height;

        SceneRenderer::OnViewportResize(width, height);

        m_Camera->SetViewportSize(width, height);

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