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

        inline Entity GetSelectedContext() const { return m_SelectionContext; }

    private:
        void DrawEntity(Entity entity);

    private:
        std::shared_ptr<Scene> m_Context;
        Entity m_SelectionContext;
    };
}

