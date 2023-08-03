#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"

#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Kaesar/ImGui/ImGuiLayer.h"

namespace Kaesar {
    class Kaesar_API Application
    {
    public:
        Application();
        virtual ~Application();
        void Run();

        void OnEvent(Event& e);

        void CloseApp();

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);

        inline Window& GetWindow() const { return *m_Window; }

        inline ImGuiLayer* GetImGuiLayer() const { return m_ImGuiLayer; }

        inline static Application& Get() { return *s_Instance; }
    private:
        bool OnWindowClose(WindowCloseEvent& e);
        bool onWindowResize(WindowResizeEvent& e);

    private:
        static Application* s_Instance; // 单例模式

        std::unique_ptr<Window> m_Window;
        ImGuiLayer* m_ImGuiLayer;

        bool m_Running = true; // 程序是否运行
        bool m_Minimized = false; // 窗口是否最小化

        float m_LastFrameTime = 0.0f; // 渲染最后一帧的时间

        LayerStack m_LayerStack;
    };

    Application* CreateApplication();
}
