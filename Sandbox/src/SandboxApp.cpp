#include <Hazel.h>

class ExampleLayer : public Hazel::Layer {
public:
    ExampleLayer()
        : Layer("Example") {}

    void OnUpdate() override 
    {
        if (Hazel::Input::IsKeyPressed(HZ_KEY_LEFT_ALT))
            HZ_TRACE("Alt key is pressed (poll)!");
    }

    void OnEvent(Hazel::Event& event) 
    {
        if (event.GetEventType() == Hazel::EventType::KeyPressed)
        {
            Hazel::KeyPressedEvent& e = static_cast<Hazel::KeyPressedEvent&>(event);
            if (e.GetKeyCode() == HZ_KEY_LEFT_ALT)
                HZ_TRACE("Alt key is pressed (event)!");
            HZ_TRACE("{0}", static_cast<char>(e.GetKeyCode()));
        }
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