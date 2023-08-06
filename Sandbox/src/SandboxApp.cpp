#include <Kaesar.h>

#include "imgui/imgui.h"

class ExampleLayer : public Kaesar::Layer {
public:
    ExampleLayer()
        : Layer("Example") 
    {
        
    }

    virtual void OnUpdate(const Kaesar::Timestep& timestep) override
    {
        
    }

    virtual void OnImGuiRender() override
    {
        
    }

    virtual void OnEvent(Kaesar::Event& event) override
    {

    }

private:
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
    Kaesar::RenderCommand::Init();
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

int main()
{
    return 0;
}