#pragma once

#include "Kaesar/Scene/Scene.h"
#include "Kaesar/Scene/Entity.h"
#include "Kaesar/Scene/Component.h"

namespace Kaesar {
    class ScenePanel
    {
    public:
        ScenePanel() = default;
        ScenePanel(const std::shared_ptr<Scene>& scene);

        void OnImGuiRender();
        inline void SetSelectedEntity(Entity entity) { m_SelectionContext = entity; }
        inline Entity GetSelectedContext() const { return m_SelectionContext; }

    private:
        void DrawEntity(Entity entity);
        void DrawComponents(Entity entity);
        void ScenePanel::DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

    private:
        std::shared_ptr<Scene> m_Context;
        Entity m_SelectionContext;

        bool m_ScaleLock = true;
    };
}

