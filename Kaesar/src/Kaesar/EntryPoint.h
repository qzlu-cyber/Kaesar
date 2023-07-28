#pragma once

#ifdef KR_PLATFORM_WINDOWS

extern Kaesar::Application* Kaesar::CreateApplication();

int main(int argc, char** argv) {
    Kaesar::Log::Init();
    KR_CORE_CRITICAL("Hello! Initialization Log!");

    Kaesar::Application* app = Kaesar::CreateApplication();
    app->Run();
    delete app;
}

#endif // HZ_PLATFORM_WINDOWS
