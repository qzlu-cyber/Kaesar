#include "hzpch.h"
#include "Application.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Log.h"

#include <glad/glad.h>

namespace Hazel {
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application* Application::s_Instance = nullptr;

    Application::Application()
    {
        HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent)); // ���ûص�����

        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);

        float vertices[] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
        };

        unsigned int indices[] = { 0, 1, 2 };

        // OpenGL ����ģʽǿ��ʹ�� VAO (�����������) 
        glGenVertexArrays(1, &m_VertexArray);
        glBindVertexArray(m_VertexArray);

        // ���������鸴�Ƶ�һ�����㻺����
        glGenBuffers(1, &m_VertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // ���������鵽һ������������
        glGenBuffers(1, &m_IndexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // �趨��������ָ��
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
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
        while (m_Running) {
            glClearColor(0.2f, 0.2f, 0.2f, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            glBindVertexArray(m_VertexArray);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

            for (Layer* layer : m_LayerStack)
                layer->OnUpdate();

            m_ImGuiLayer->Begin();
            for (Layer* layer : m_LayerStack)
                layer->OnImGuiRender();
            m_ImGuiLayer->End();

            m_Window->OnUpdate();
        }
    }

    bool Application::OnWindowClose(WindowCloseEvent& e) 
    {
        m_Running = false;
        return true;
    }
}