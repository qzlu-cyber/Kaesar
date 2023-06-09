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

// ���������ڵ��ƶ�������Ĵ����У�Sandbox ֻ�۽�ʵ��Ӧ��
Hazel::Application* Hazel::CreateApplication() 
{
    return new Sandbox();
}