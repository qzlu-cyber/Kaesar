#include <Kaesar.h>

#include "imgui/imgui.h"

class ExampleLayer : public Kaesar::Layer {
public:
    ExampleLayer()
        : Layer("Example") 
    {
        float vertices[] = {
            -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
             0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
             0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
        };

        float cubeVertices[] = {
            -0.5f, -0.5f, -0.5f,  0.2f, 0.8f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.3f, 0.8f, 1.0f,
             0.5f,  0.5f, -0.5f,  0.8f, 0.2f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.3f, 0.8f, 1.0f,

            -0.5f, -0.5f,  0.5f,  0.2f, 0.8f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.3f, 0.8f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.8f, 0.2f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.3f, 0.8f, 1.0f,
        };

        uint32_t cubeIndices[] = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 4, 7,
            7, 3, 0, 0, 4, 7,
            6, 2, 1, 1, 5, 6,
            0, 1, 5, 5, 4, 0,
            3, 2, 6, 6, 7, 3
        };

        uint32_t indices[] = { 0, 1, 2 };
        
        m_VertexArray.reset(Kaesar::VertexArray::Create());

        // 将顶点数组复制到一个顶点缓冲中
        std::shared_ptr<Kaesar::VertexBuffer> vertexBuffer;
        vertexBuffer.reset(Kaesar::VertexBuffer::Create(cubeVertices, sizeof(cubeVertices)));

        // 将索引数组到一个索引缓冲中
        std::shared_ptr<Kaesar::IndexBuffer> indexBuffer;
        indexBuffer.reset(Kaesar::IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(uint32_t)));

        Kaesar::BufferLayout layout = {
            { Kaesar::ShaderDataType::Float3, "a_Position" },
            { Kaesar::ShaderDataType::Float3, "a_Color" }
        };

        vertexBuffer->SetLayout(layout);

        m_VertexArray->AddVertexBuffer(vertexBuffer);
        m_VertexArray->SetIndexBuffer(indexBuffer);

        const std::string basicShaderPath = "D:\\CPP\\Kaesar\\Kaesar\\src\\res\\shaders\\basic.shader";
        m_Shader.reset(new Kaesar::Shader(basicShaderPath));

        float squareVertices[3 * 4] = {
            -0.75f, -0.75f, 0.0f,
             0.75f, -0.75f, 0.0f,
             0.75f,  0.75f, 0.0f,
            -0.75f,  0.75f, 0.0f
        };

        uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

        m_SquareVA.reset(Kaesar::VertexArray::Create());

        std::shared_ptr<Kaesar::VertexBuffer> squareVB;
        squareVB.reset(Kaesar::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
        squareVB->SetLayout({
            { Kaesar::ShaderDataType::Float3, "a_Position" }
            });
        m_SquareVA->AddVertexBuffer(squareVB);

        std::shared_ptr<Kaesar::IndexBuffer> squareIB;
        squareIB.reset(Kaesar::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
        m_SquareVA->SetIndexBuffer(squareIB);

        const std::string blueShaderPath = "D:\\CPP\\Kaesar\\Kaesar\\src\\res\\shaders\\blue.shader";
        m_BlueShader.reset(new Kaesar::Shader(blueShaderPath));

        m_Camera = std::make_shared<Kaesar::PerspectiveCamera>(45.0f, 1.66f, 0.1f, 100.0f);
        m_Camera->SetViewportSize(1600, 900);
    }

    void OnUpdate() override
    {
        if (Kaesar::Input::IsKeyPressed(KR_KEY_LEFT_ALT))
            KR_TRACE("Alt key is pressed (poll)!");

        Kaesar::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
        Kaesar::RenderCommand::Clear();
        Kaesar::RenderCommand::DepthTest();

        m_Camera->OnUpdate();

        Kaesar::Renderer::BeginScene();

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = m_Camera->GetViewMatrix();
        glm::mat4 projection = m_Camera->GetProjection();
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.3f, 0.5f));

        m_BlueShader->Bind();

        m_BlueShader->SetMatrix("model", model);
        m_BlueShader->SetMatrix("view", view);
        m_BlueShader->SetMatrix("projection", projection);

        Kaesar::Renderer::Submit(m_SquareVA);

        m_Shader->Bind();

        m_Shader->SetMatrix("model", model);
        m_Shader->SetMatrix("view", view);
        m_Shader->SetMatrix("projection", projection);

        Kaesar::Renderer::Submit(m_VertexArray);

        Kaesar::Renderer::EndScene();
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

private:
    std::shared_ptr<Kaesar::Shader> m_Shader;
    std::shared_ptr<Kaesar::VertexArray> m_VertexArray;

    std::shared_ptr<Kaesar::Shader> m_BlueShader;
    std::shared_ptr<Kaesar::VertexArray> m_SquareVA;

    std::shared_ptr<Kaesar::PerspectiveCamera> m_Camera;
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