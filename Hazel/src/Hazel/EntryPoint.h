#pragma once

#ifdef HZ_PLATFORM_WINDOWS

extern Hazel::Application* Hazel::CreateApplication();

int main(int argc, char** argv) {
    Hazel::Log::Init();
    HZ_CORE_CRITICAL("Hello! Initialization Log!");

    Hazel::Application* app = Hazel::CreateApplication();
    app->Run();
    delete app;
}

#endif // HZ_PLATFORM_WINDOWS
