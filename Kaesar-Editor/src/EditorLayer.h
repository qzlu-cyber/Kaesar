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

        void ShowIcons();

    private:
        std::shared_ptr<Scene> m_ActiveScene;
        std::shared_ptr<ScenePanel> m_ScenePanel;

        int m_GizmoType = -1; // -1: none, 0: translate, 1: rotate, 2: scale
        int m_GizmoMode = 0; // 0: local, 1: world
        bool m_GizmosChanged = true; // ��� gizmos �Ƿ񱻸ı�

        glm::vec2 m_ViewportSize = { 200.0f, 200.0f };

        bool m_ViewportFocused = false; // ����ӿ��Ƿ񱻾۽�
        bool m_ViewportHovered = false; // �������Ƿ����ӿ���

        glm::vec2 m_ViewportBounds[2];

        std::string m_Info;
        Entity m_SelectedEntity;

        std::shared_ptr<Texture2D> m_GizmosIcon;
        std::shared_ptr<Texture2D> m_TransformIcon;
        std::shared_ptr<Texture2D> m_RotationIcon;
        std::shared_ptr<Texture2D> m_ScaleIcon;
    };
}