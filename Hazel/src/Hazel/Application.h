#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Window.h"

namespace Hazel {

    class HAZEL_API Application
    {
    public:
        Application();
        virtual ~Application();
        void Run();
        bool m_Running = true;
    private:
        std::unique_ptr<Window> m_Window;
    };

    Application* CreateApplication();
}

