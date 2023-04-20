#include "hzpch.h"
#include "Application.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Log.h"

#include <GLFW/glfw3.h>

namespace Hazel {

    Application::Application()
    {
        m_Window = std::unique_ptr<Window>(Window::Create());
    }

    Application::~Application()
    {
    }

    void Application::Run() {
        std::cout << "Hello World!" << std::endl;

        /*WindowResizeEvent e(1280, 720);
        if (e.IsInCategory(EventCategoryApplication))
        {
            HZ_TRACE(e);
        }
        if (e.IsInCategory(EventCategoryInput))
        {
            HZ_TRACE(e);
        }*/

        while (m_Running) {
            glClearColor(1, 0, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            m_Window->OnUpdate();
        }
    }

}