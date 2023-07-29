#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"

#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Kaesar/ImGui/ImGuiLayer.h"
#include "Kaesar/Renderer/Shader.h"
#include "Kaesar/Renderer/Buffer.h"
#include "Kaesar/Renderer/VertexArray.h"
#include "Kaesar/Renderer/Camera/PerspectiveCamera.h"

namespace Kaesar {
    class Kaesar_API Application
    {
    public:
        Application();
        virtual ~Application();
        void Run();

        void OnEvent(Event& e);

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);

        inline Window& GetWindow() { return *m_Window; }

        inline static Application& Get() { return *s_Instance; }
    private:
        bool OnWindowClose(WindowCloseEvent& e);
        bool onWindowResize(WindowResizeEvent& e);

    private:
        static Application* s_Instance; // ����ģʽ

        std::unique_ptr<Window> m_Window;
        ImGuiLayer* m_ImGuiLayer;

        bool m_Running = true; // �����Ƿ�����
        bool m_Minimized = false; // �����Ƿ���С��

        LayerStack m_LayerStack;

        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<VertexArray> m_VertexArray;

        std::shared_ptr<Shader> m_BlueShader;
        std::shared_ptr<VertexArray> m_SquareVA;

        std::shared_ptr<PerspectiveCamera> m_Camera;
    };

    Application* CreateApplication();
}
