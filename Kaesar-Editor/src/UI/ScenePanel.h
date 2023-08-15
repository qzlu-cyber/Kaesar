#pragma once

#include "Kaesar/Scene/Scene.h"
#include "Kaesar/Scene/Entity.h"
#include "Kaesar/Scene/Component.h"

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
        void DrawComponents(Entity entity);
        void ScenePanel::DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

    private:
        std::shared_ptr<Scene> m_Context;
        Entity m_SelectionContext;

        ShaderLibrary m_Shaders;
        std::vector<std::string> m_ShaderNames;

        ImTextureID m_TextureId;
        std::shared_ptr<Texture2D> m_EmptyTexture;

        std::string m_SelectedShader;

        bool m_ScaleLock = true;
        bool m_EntityCreated = false;
    };
}

