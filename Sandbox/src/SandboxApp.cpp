#include <Kaesar.h>

#include "imgui/imgui.h"

class ExampleLayer : public Kaesar::Layer {
public:
    ExampleLayer()
        : Layer("Example") 
    {
        auto& app = Kaesar::Application::Get();

        float vertices[] = {
            // back face
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f,-1.0f, // bottom-left
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, 0.0f,-1.0f, // bottom-right    
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f,-1.0f, // top-right              
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f,-1.0f, // top-right
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f,-1.0f, // top-left
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f,-1.0f, // bottom-left                
            // front face
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f,1.0f,  // bottom-left
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f,1.0f,  // top-right
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 0.0f,1.0f,  // bottom-right        
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f,1.0f,  // top-right
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f,1.0f,  // bottom-left
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 0.0f,1.0f,  // top-left        
            // left face
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f, 0.0f,0.0f, // top-right
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f, 0.0f,0.0f, // bottom-left
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  -1.0f, 0.0f,0.0f, // top-left       
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f, 0.0f,0.0f, // bottom-left
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f, 0.0f,0.0f, // top-right
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  -1.0f, 0.0f,0.0f, // bottom-right
            // right face
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f, 0.0f,0.0f,  // top-left
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f, 0.0f,0.0f,  // top-right      
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f, 0.0f,0.0f,  // bottom-right          
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f, 0.0f,0.0f,  // bottom-right
             0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f, 0.0f,0.0f,  // bottom-left
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f, 0.0f,0.0f,  // top-left
             // bottom face          
             -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,0.0f, // top-right
              0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,0.0f, // bottom-left
              0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,0.0f, // top-left        
              0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,0.0f, // bottom-left
             -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,0.0f, // top-right
             -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,0.0f, // bottom-right
             // top face										     
             -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 1.0f,0.0f,  // top-left
              0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 1.0f,0.0f,  // top-right
              0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 1.0f,0.0f,  // bottom-right                 
              0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 1.0f,0.0f,  // bottom-right
             -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 1.0f,0.0f,  // bottom-left  
             -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 1.0f,0.0f   // top-left 
        };

        float quad[] = {
            // positions        // texture coords
             1.0f,  1.0f, 0.0f,    1.0f, 1.0f,   // top right
             1.0f, -1.0f, 0.0f,    1.0f, 0.0f,   // bottom right
            -1.0f, -1.0f, 0.0f,    0.0f, 0.0f,   // bottom left
            -1.0f,  1.0f, 0.0f,    0.0f, 1.0f    // top left 
        };

        uint32_t indices[] = { 
            // front
            0, 1, 2,
            2, 3, 0,
            // right
            1, 5, 6,
            6, 2, 1,
            // back
            7, 6, 5,
            5, 4, 7,
            // left
            4, 0, 3,
            3, 7, 4,
            // bottom
            4, 5, 1,
            1, 0, 4,
            // top
            3, 2, 6,
            6, 7, 3
        };

        unsigned int quadIndices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };
        
        m_VertexArray = Kaesar::VertexArray::Create();
        m_QuadVA = Kaesar::VertexArray::Create();

        // 将顶点数组复制到一个顶点缓冲中
        m_VertexBuffer = Kaesar::VertexBuffer::Create(&vertices[0], sizeof(vertices));
        m_QuadVB = Kaesar::VertexBuffer::Create(&quad[0], sizeof(quad));

        // 将索引数组到一个索引缓冲中
        m_IndexBuffer = Kaesar::IndexBuffer::Create(&indices[0], sizeof(indices) / sizeof(uint32_t));
        m_QuadIB = Kaesar::IndexBuffer::Create(&quadIndices[0], sizeof(quadIndices) / sizeof(uint32_t));

        Kaesar::BufferLayout layout = {
            { Kaesar::ShaderDataType::Float3, "a_Position" },
            { Kaesar::ShaderDataType::Float3, "a_Normal" },
            { Kaesar::ShaderDataType::Float2, "a_UV" }
        };

        Kaesar::BufferLayout quadLayout = {
            { Kaesar::ShaderDataType::Float3, "a_Position" },
            { Kaesar::ShaderDataType::Float2, "a_UV" }
        };

        m_VertexBuffer->SetLayout(layout);
        m_QuadVB->SetLayout(quadLayout);

        m_VertexArray->AddVertexBuffer(m_VertexBuffer);
        m_VertexArray->SetIndexBuffer(m_IndexBuffer);

        m_QuadVA->AddVertexBuffer(m_QuadVB);
        m_QuadVA->SetIndexBuffer(m_QuadIB);

        const std::string basicShaderPath = "D:\\CPP\\Kaesar\\Kaesar\\src\\res\\shaders\\basic.glsl";
        m_Shader = Kaesar::Shader::Create(basicShaderPath);

        const std::string blueShaderPath = "D:\\CPP\\Kaesar\\Kaesar\\src\\res\\shaders\\basic.glsl";
        m_QuadShader = Kaesar::Shader::Create(blueShaderPath);

        m_Model = std::make_shared<Kaesar::Model>("D:\\CPP\\Kaesar\\Kaesar\\src\\res\\models\\spot\\spot.obj");

        m_Texture = Kaesar::Texture2D::Create("D:\\CPP\\Kaesar\\Kaesar\\src\\res\\models\\spot\\spot_texture.png", 0);

        m_Camera = std::make_shared<Kaesar::PerspectiveCamera>(45.0f, 1.66f, 0.1f, 100.0f);
        m_Camera->SetViewportSize(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());
    }

    virtual void OnUpdate(const Kaesar::Timestep& timestep) override
    {
        if (Kaesar::Input::IsKeyPressed(KR_KEY_LEFT_ALT))
            KR_TRACE("Alt key is pressed (poll)!");

        Kaesar::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
        Kaesar::RenderCommand::Clear();
        Kaesar::RenderCommand::DepthTest();

        m_Camera->OnUpdate(timestep);

        Kaesar::Renderer::BeginScene();

        glm::mat4 model = glm::mat4(1.0f);

        /// ====================== spot ========================
        m_Texture->Active(0);
        m_Texture->Bind();

        m_Shader->Bind(); // glUseProgram
        m_Shader->SetMat4("u_Model", model);
        m_Shader->SetMat4("u_ViewProjection", m_Camera->GetViewProjection());

        Kaesar::Renderer::Submit(m_Model);
        /// ====================== spot end =====================

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
    std::shared_ptr<Kaesar::VertexArray> m_VertexArray, m_QuadVA;
    std::shared_ptr<Kaesar::VertexBuffer> m_VertexBuffer, m_QuadVB;
    std::shared_ptr<Kaesar::IndexBuffer> m_IndexBuffer, m_QuadIB;
    std::shared_ptr<Kaesar::Texture2D> m_Texture;

    std::shared_ptr<Kaesar::Model> m_Model;

    std::shared_ptr<Kaesar::Shader> m_Shader, m_QuadShader;

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