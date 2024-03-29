#pragma once

#include "Kaesar/Scene/Scene.h"

#include <entt.hpp>

namespace Kaesar {
    class Entity
    {
    public:
        Entity() = default;

        Entity(entt::entity handle);

        Entity(const Entity& other) = default;

        ~Entity() = default;

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            KR_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
            T& component = s_Scene->m_Registry.emplace<T>(m_EntityID, std::forward<Args>(args)...);
            return component;
        }

        template<typename T>
        T& GetComponent()
        {
            KR_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            return s_Scene->m_Registry.get<T>(m_EntityID);
        }

        template<typename T>
        bool HasComponent()
        {
            return s_Scene->m_Registry.has<T>(m_EntityID);
        }

        template<typename T>
        void RemoveComponent()
        {
            KR_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            s_Scene->m_Registry.remove<T>(m_EntityID);
        }

        bool IsSelected() const { return m_Selected; }

        void SetSelected(bool selected) { m_Selected = selected; }

        bool operator ==(const Entity& other) const {
            return m_EntityID == other.m_EntityID;
        }

        bool operator ==(const entt::entity& other) const {
            return other == m_EntityID;
        }

        bool operator !=(const Entity& other) {
            return !(*this == other);
        }

        operator bool() const {
            return m_EntityID != entt::null;
        }

        operator entt::entity() const {
            return m_EntityID;
        }

        operator uint32_t() const {
            return (uint32_t)m_EntityID;
        }

    public:
        static Scene* s_Scene;

    private:
        entt::entity m_EntityID{ entt::null };

        bool m_Selected;
    };
}

