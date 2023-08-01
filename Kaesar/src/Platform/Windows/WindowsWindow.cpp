#include "krpch.h"
#include "WindowsWindow.h"

#include "Kaesar/Core/Log.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/ApplicationEvent.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include <glad/glad.h>

namespace Kaesar {
    static bool s_GLFWInitialized = false; // 用来确定 GLFW 调用的时候被初始化过了，否则会报错

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
        m_Context->Init(); // 初始化 OpenGLContext

        // 这里本质上是绑定了一个用户自定义的指针到 window，签名里是个 void*，根据文档，这就是
        // 一个用户自己爱干嘛干嘛的入口，glfw 本身不会对这个指针做任何操作，我们可以把对应的信息传进去
        glfwSetWindowUserPointer(m_Window, &m_Data);
        SetVSync(true); // 默认开启垂直同步

        /// 设置 GLFW 回调
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
            {
                // 在 lambda 中调用 Application 绑定的回调函数
                WindowData& data = *(static_cast<WindowData*>(glfwGetWindowUserPointer(window))); // 先转成正确的指针类型再解引用

                // 更新窗口大小
                data.Width = width;
                data.Height = height;

                // 调用事件
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
        // 每次 update 时，处理当前在队列中的事件
        glfwPollEvents();
        // 刷新下一帧 (严格来说是把 Framebuffer 后台帧换到前台，把 Framebuffer 当前帧换到后台，所以是 Swap)
        m_Context->SwapBuffers();
    }

    void WindowsWindow::SetVSync(bool enabled)
    {
        // If w/out brace, only 1 closest line will be executed
        // 这里的 1/0 并不是开/关的意思，而是下一帧 Swap 之间要等多少次 Screen Update
        // 但是 glfwSwapInterval 内部封装了对应平台的 vsync 函数，所以基本上就是开关的意思
        // 要注意的是，我们可以把这个 interval 开成10，然后视窗看起来会巨卡，但依然 VSync on
        // 可见，VSync 跟实际渲染的帧数没有任何关系，VSync 看似锁 60 帧只是因为大部分显示器刷新率 60HZ，被 “Sync” 了
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