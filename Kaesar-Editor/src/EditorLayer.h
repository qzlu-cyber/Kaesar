#pragma once

#include <Kaesar.h>

#include "UI/ScenePanel.h"
#include "Kaesar/Core/Layer.h"

namespace Kaesar {
    class EditorLayer : public Layer
    {
    public:
        EditorLayer();
        ~EditorLayer();

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnImGuiRender() override;
        virtual void OnUpdate(const Timestep& timestep) override;
        virtual void OnEvent(Event& event) override;

    private:
        void NewScene();
        void OpenScene();
        void SaveSceneAs();

        bool OnKeyPressed(KeyPressedEvent& e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

    private:
        std::shared_ptr<Scene> m_ActiveScene;
        std::shared_ptr<ScenePanel> m_ScenePanel;

        PerspectiveCamera* m_Camera;

        int m_GizmoType = -1;

        glm::vec2 m_ViewportSize = { 200.0f, 200.0f };
        glm::vec3 m_ClearColor = { 0.1f, 0.1f, 0.1f };
        glm::vec3 m_CubeColor = { 1.0f, 1.0f, 1.0f };
        glm::vec3 m_Scale = { 1.0f,1.0f,1.0f };

        bool m_ViewportFocused = false; // 标记视口是否被聚焦
        bool m_ViewportHovered = false; // 标记鼠标是否在视口上

        glm::vec2 m_ViewportBounds[2];

        std::string m_Info;
        Entity m_SelectedEntity;
    };
}