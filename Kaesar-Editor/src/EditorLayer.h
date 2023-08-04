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
        std::shared_ptr<Scene> m_ActiveScene;
        std::shared_ptr<ScenePanel> m_ScenePanel;
        std::shared_ptr<VertexArray> m_QuadVA;
        std::shared_ptr<VertexBuffer> m_QuadVB;
        std::shared_ptr<IndexBuffer> m_QuadIB;
        std::shared_ptr<FrameBuffer> m_FrameBuffer, m_PostProcessingFB;

        std::shared_ptr<Texture2D> m_Texture;

        std::shared_ptr<Model> m_Model;

        std::shared_ptr<Shader> m_Shader, m_QuadShader;

        std::shared_ptr<PerspectiveCamera> m_Camera;

        glm::vec2 m_ViewportSize = { 200.0f, 200.0f };
        glm::vec3 m_ClearColor = { 0.1f, 0.1f, 0.1f };
        glm::vec3 m_CubeColor = { 1.0f, 1.0f, 1.0f };
        glm::vec3 m_Scale = { 1.0f,1.0f,1.0f };

        bool m_ViewportFocused = false; // 标记视口是否被聚焦
        bool m_ViewportHovered = false; // 标记鼠标是否在视口上

    };
}