#include <Kaesar.h>

#include "imgui/imgui.h"

class ExampleLayer : public Kaesar::Layer {
public:
    ExampleLayer()
        : Layer("Example") {}

    void OnUpdate() override
    {
        if (Kaesar::Input::IsKeyPressed(KR_KEY_LEFT_ALT))
            KR_TRACE("Alt key is pressed (poll)!");
    }

    virtual void OnImGuiRender() override
    {
        ImGui::Begin("Test");
        ImGui::Text("Hello World");
        ImGui::End();
    }

    virtual void OnEvent(Kaesar::Event& event) override
    {
        if (event.GetEventType() == Kaesar::EventType::KeyPressed)
        {
            Kaesar::KeyPressedEvent& e = static_cast<Kaesar::KeyPressedEvent&>(event);
            if (e.GetKeyCode() == KR_KEY_LEFT_ALT)
                KR_TRACE("Alt key is pressed (event)!");
            KR_TRACE("{0}", static_cast<char>(e.GetKeyCode()));
        }
    }
};

class Sandbox : public Kaesar::Application
{
public:
    Sandbox();
    ~Sandbox();

private:
};

Sandbox::Sandbox()
{
    Application::PushLayer(new ExampleLayer());
}

Sandbox::~Sandbox()
{
}

// 将引擎的入口点移动到引擎的代码中，Sandbox 只聚焦实际应用
Kaesar::Application* Kaesar::CreateApplication()
{
    return new Sandbox();
}