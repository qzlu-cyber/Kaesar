#include <Hazel.h>

class ExampleLayer : public Hazel::Layer {
public:
    ExampleLayer()
        : Layer("Example") {}

    void OnUpdate() override 
    {
        HZ_INFO("ExampleLayer::Update");
    }

    void OnEvent(Hazel::Event& e) 
    {
        HZ_TRACE("{0}", e);
    }
};

class Sandbox : public Hazel::Application
{
public:
    Sandbox();
    ~Sandbox();

private:
};

Sandbox::Sandbox() 
{
    PushLayer(new ExampleLayer());
    PushOverlay(new Hazel::ImGuiLayer());
}

Sandbox::~Sandbox() 
{
}

// 将引擎的入口点移动到引擎的代码中，Sandbox 只聚焦实际应用
Hazel::Application* Hazel::CreateApplication() 
{
    return new Sandbox();
}