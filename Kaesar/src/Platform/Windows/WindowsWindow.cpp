#include "krpch.h"
#include "WindowsWindow.h"

#include "Kaesar/Core/Log.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/ApplicationEvent.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include <glad/glad.h>

namespace Kaesar {
    static bool s_GLFWInitialized = false; // ����ȷ�� GLFW ���õ�ʱ�򱻳�ʼ�����ˣ�����ᱨ��

    static void GLFWErrorCallback(int error, const char* description) {
        KR_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
    }

    Window* Window::Create(const WindowProps& props) {
        return new WindowsWindow(props);
    }

    WindowsWindow::WindowsWindow(const WindowProps& props) {
        Init(props);
    }

    WindowsWindow::~WindowsWindow() {
        Shutdown();
    }

    void WindowsWindow::Init(const WindowProps& props) {
        m_Data.Title = props.Title;
        m_Data.Width = props.Width;
        m_Data.Height = props.Height;

        KR_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

        if (!s_GLFWInitialized) {
            int success = glfwInit();
            KR_CORE_ASSERT(success, "Could not intialize GLFW!");

            glfwSetErrorCallback(GLFWErrorCallback);

            s_GLFWInitialized = true;
        }

        m_Window = glfwCreateWindow(static_cast<int>(props.Width), static_cast<int>(props.Height), m_Data.Title.c_str(), nullptr, nullptr);

        m_Context = new OpenGLContext(m_Window);
        m_Context->Init(); // ��ʼ�� OpenGLContext

        // ���ﱾ�����ǰ���һ���û��Զ����ָ�뵽 window��ǩ�����Ǹ� void*�������ĵ��������
        // һ���û��Լ�������������ڣ�glfw ����������ָ�����κβ��������ǿ��԰Ѷ�Ӧ����Ϣ����ȥ
        glfwSetWindowUserPointer(m_Window, &m_Data);
        SetVSync(true); // Ĭ�Ͽ�����ֱͬ��

        /// ���� GLFW �ص�
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
            {
                // �� lambda �е��� Application �󶨵Ļص�����
                WindowData& data = *(static_cast<WindowData*>(glfwGetWindowUserPointer(window))); // ��ת����ȷ��ָ�������ٽ�����

                // ���´��ڴ�С
                data.Width = width;
                data.Height = height;

                // �����¼�
                WindowResizeEvent event(width, height);
                data.EventCallback(event);
            }
        );

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
            {
                WindowData& data = *(static_cast<WindowData*>(glfwGetWindowUserPointer(window)));

                WindowCloseEvent event;
                data.EventCallback(event);
            }
        );

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
            {
                WindowData& data = *(static_cast<WindowData*>(glfwGetWindowUserPointer(window)));

                switch (action)
                {
                case GLFW_PRESS:
                {
                    KeyPressedEvent event(key, 0);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    KeyReleasedEvent event(key);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_REPEAT:
                {
                    KeyPressedEvent event(key, 1);
                    data.EventCallback(event);
                    break;
                }
                default:
                    break;
                }
            }
        );

        glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
            {
                WindowData& data = *(static_cast<WindowData*>(glfwGetWindowUserPointer(window)));

                KeyTypedEvent event(keycode);
                data.EventCallback(event);
            }
        );

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
            {
                WindowData& data = *(static_cast<WindowData*>(glfwGetWindowUserPointer(window)));

                switch (action)
                {
                case GLFW_PRESS:
                {
                    MouseButtonPressedEvent event(button);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    MouseButtonReleasedEvent event(button);
                    data.EventCallback(event);
                    break;
                }
                }
            }
        );

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
            {
                WindowData& data = *(static_cast<WindowData*>(glfwGetWindowUserPointer(window)));

                MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
                data.EventCallback(event);
            }
        );

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
            {
                WindowData& data = *(static_cast<WindowData*>(glfwGetWindowUserPointer(window)));

                MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
                data.EventCallback(event);
            }
        );
    }

    void WindowsWindow::Shutdown()
    {
        glfwDestroyWindow(m_Window);
    }

    void WindowsWindow::OnUpdate()
    {
        // ÿ�� update ʱ������ǰ�ڶ����е��¼�
        glfwPollEvents();
        // ˢ����һ֡ (�ϸ���˵�ǰ� Framebuffer ��̨֡����ǰ̨���� Framebuffer ��ǰ֡������̨�������� Swap)
        m_Context->SwapBuffers();
    }

    void WindowsWindow::SetVSync(bool enabled)
    {
        // If w/out brace, only 1 closest line will be executed
        // ����� 1/0 �����ǿ�/�ص���˼��������һ֡ Swap ֮��Ҫ�ȶ��ٴ� Screen Update
        // ���� glfwSwapInterval �ڲ���װ�˶�Ӧƽ̨�� vsync ���������Ի����Ͼ��ǿ��ص���˼
        // Ҫע����ǣ����ǿ��԰���� interval ����10��Ȼ���Ӵ���������޿�������Ȼ VSync on
        // �ɼ���VSync ��ʵ����Ⱦ��֡��û���κι�ϵ��VSync ������ 60 ֻ֡����Ϊ�󲿷���ʾ��ˢ���� 60HZ���� ��Sync�� ��
        if (enabled)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);

        m_Data.VSync = enabled;
    }

    bool WindowsWindow::IsVSync() const
    {
        return m_Data.VSync;
    }
}