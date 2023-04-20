#include "hzpch.h"
#include "Application.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Log.h"

namespace Hazel {

    Application::Application()
    {
    }

    Application::~Application()
    {
    }

    void Application::Run() {
        std::cout << "Hello World!" << std::endl;

        WindowResizeEvent e(1280, 720);
        if (e.IsInCategory(EventCategoryApplication))
        {
            HZ_TRACE(e);
        }
        if (e.IsInCategory(EventCategoryInput))
        {
            HZ_TRACE(e);
        }

        std::cin.get();
    }

}