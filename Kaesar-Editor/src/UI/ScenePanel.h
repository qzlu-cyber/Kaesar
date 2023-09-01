#pragma once

#include "Kaesar/Scene/Scene.h"
#include "Kaesar/Scene/Entity.h"
#include "Kaesar/Scene/Component.h"

#include "Panels/MeshPanel.h"
#include "Panels/MaterialPanel.h"
#include "Panels/LightPanel.h"
#include "Panels/CameraPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

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
        void DrawEntity(std::shared_ptr<Entity>& entity);
        void DrawComponents(Entity& entity);

    private:
        std::shared_ptr<Scene> m_Context;
        Entity m_SelectionContext;

        ShaderLibrary m_Shaders;
        std::vector<std::string> m_ShaderNames;

        std::string m_SelectedShader;

        std::shared_ptr<MeshPanel> m_MeshPanel;
        std::shared_ptr<MaterialPanel> m_MaterialPanel;
        std::shared_ptr<LightPanel> m_LightPanel;
        std::shared_ptr< CameraPanel> m_CameraPanel;

        bool m_EntityCreated = false;
    };
}

