#include "krpch.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Kaesar/Renderer/Renderer.h"

#include "Log.h"
#include "Application.h"

namespace Kaesar {
    #define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application* Application::s_Instance = nullptr;

    Application::Application()
    {
        KR_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent)); // ���ûص�����

        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);
    }

    Application::~Application()
    {
    }

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* overlay)
    {
        m_LayerStack.PushOverlay(overlay);
        overlay->OnAttach();
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose)); // ����¼� e �� EventType �Ƿ��� WindowClose������Ǿ�ִ�� OnWindowClose ����
        dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(onWindowResize));

        // ����������ʱ������������� LayerStack�����β鿴�¼��Ƿ���� Layer ����Ӧ��
        // �������������У��¼��� handle �ˣ���ô��ֹͣ���²� layer ���ݣ��������������
        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
        {
            (*--it)->OnEvent(e);
            if (e.m_Handled)
                break;
        }
    }

    void Application::Run()
    {

        float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
        float lastFrame = 0.0f; // ��һ֡��ʱ��
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        while (m_Running) {
            for (Layer* layer : m_LayerStack)
                layer->OnUpdate();

            m_ImGuiLayer->Begin();
            for (Layer* layer : m_LayerStack)
                layer->OnImGuiRender();
            m_ImGuiLayer->End();

            m_Window->OnUpdate();
        }
    }

    bool Application::onWindowResize(WindowResizeEvent& e)
    {
        if (e.GetWidth() == 0 && e.GetHeight() == 0)
        {
            m_Minimized = true;
            return false;
        }

        m_Minimized = false;
        Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
        return false;
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        m_Running = false;
        return true;
    }
}