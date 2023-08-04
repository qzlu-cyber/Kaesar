#include "krpch.h"
#include "Scene.h"

#include "Kaesar/Scene/Entity.h"
#include "Kaesar/Scene/Component.h"

namespace Kaesar {
    Scene::Scene() {}
    Scene::~Scene() {}

    Entity Scene::CreateEntity(const std::string& name) {
        Entity entity = { m_Registry.create(), this };

        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;
        entity.AddComponent<TransformComponent>();

        return entity;
    }

    void Scene::DestroyEntity(Entity entity) {
        m_Registry.destroy(entity);
    }

    void Scene::OnUpdateRuntime(Timestep ts) {
        //TODO: Implement OnUpdateRuntime
    }

    void Scene::OnUpdateEditor(Timestep ts, std::shared_ptr<PerspectiveCamera>& camera) {
        //TODO: Implement OnUpdateEditor
        auto group = m_Registry.group<TransformComponent>(entt::get<TagComponent>);
        for (auto& entity : group)
        {
            auto tag = m_Registry.get<TagComponent>(entity);
            //KR_CORE_TRACE("Entity with ID :{0} and tag name : {1}", (uint32_t)entity, tag.Tag);
        }
    }

    void Scene::OnViewportResize(uint32_t width, uint32_t height) {
        //TODO: Implement OnViewportResize
    }
}