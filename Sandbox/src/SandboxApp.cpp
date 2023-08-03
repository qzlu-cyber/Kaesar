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
            { Kaesar::ShaderDataType::Float2, "a_TexCoords" }
        };

        Kaesar::BufferLayout quadLayout = {
            { Kaesar::ShaderDataType::Float2, "a_Position" },
            { Kaesar::ShaderDataType::Float2, "a_TexCoords" }
        };

        m_VertexBuffer->SetLayout(layout);
        m_QuadVB->SetLayout(quadLayout);

        m_VertexArray->AddVertexBuffer(m_VertexBuffer);
        m_VertexArray->SetIndexBuffer(m_IndexBuffer);

        m_QuadVA->AddVertexBuffer(m_QuadVB);
        m_QuadVA->SetIndexBuffer(m_QuadIB);

        Kaesar::FramebufferSpecification fspc;
        fspc.Width = app.GetWindow().GetWidth();
        fspc.Height = app.GetWindow().GetHeight();

        m_ViewportSize = { fspc.Width, fspc.Height };

        fspc.Samples = 4; // 4x MSAA
        m_FrameBuffer = Kaesar::FrameBuffer::Create(fspc); // MSAA framebuffer
        fspc.Samples = 1;
        m_PostProcessingFB = Kaesar::FrameBuffer::Create(fspc); // 后处理 framebuffer

        const std::string basicShaderPath = "D:\\CPP\\Kaesar\\Kaesar\\src\\res\\shaders\\basic.glsl";
        m_Shader = Kaesar::Shader::Create(basicShaderPath);

        const std::string quadShaderPath = "D:\\CPP\\Kaesar\\Kaesar\\src\\res\\shaders\\quad.glsl";
        m_QuadShader = Kaesar::Shader::Create(quadShaderPath);
        m_QuadShader->SetInt("u_Texture", 0);

        m_Model = std::make_shared<Kaesar::Model>("D:\\CPP\\Kaesar\\Kaesar\\src\\res\\models\\spot\\spot.obj");

        m_Texture = Kaesar::Texture2D::Create("D:\\CPP\\Kaesar\\Kaesar\\src\\res\\models\\spot\\spot_texture.png", 0);

        m_Camera = std::make_shared<Kaesar::PerspectiveCamera>(45.0f, 1.778f, 0.1f, 100.0f);
        m_Camera->SetViewportSize((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());
    }

    virtual void OnUpdate(const Kaesar::Timestep& timestep) override
    {
        Kaesar::FramebufferSpecification spec = m_FrameBuffer->GetSpecification();
        if ( m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
            (spec.Width != (uint32_t)m_ViewportSize.x || spec.Height != (uint32_t)m_ViewportSize.y))
        {
            m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_PostProcessingFB->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_Camera->SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
        }

        if (Kaesar::Input::IsKeyPressed(KR_KEY_LEFT_ALT))
            KR_TRACE("Alt key is pressed (poll)!");

        m_FrameBuffer->Bind();

        Kaesar::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        Kaesar::RenderCommand::Clear();
        Kaesar::RenderCommand::EnableDepthTest();

        if (m_ViewportFocused) // 只有窗口聚焦时才更新相机
        {
            m_Camera->OnUpdate(timestep);
        }

        Kaesar::Renderer::BeginScene();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(140.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));

        /// ====================== spot ========================
        m_Texture->Active(0);
        m_Texture->Bind();

        m_Shader->Bind(); // glUseProgram
        m_Shader->SetMat4("u_Model", model);
        m_Shader->SetMat4("u_ViewProjection", m_Camera->GetViewProjection());

        Kaesar::Renderer::Submit(m_Model);
        /// ====================== spot end =====================
        m_Texture->Unbind();
        m_Shader->Unbind();
        m_FrameBuffer->Unbind();

        Kaesar::Renderer::EndScene();

        m_FrameBuffer->BlitMultiSample(m_FrameBuffer->GetRendererID(), m_PostProcessingFB->GetRendererID());
        
        m_FrameBuffer->Unbind();
        Kaesar::RenderCommand::SetClearColor({ 1.0f, 1.0f, 1.0f, 1.0f });
        Kaesar::RenderCommand::ClearColor();
        Kaesar::RenderCommand::DisableDepthTest();

        m_QuadShader->Bind();
        m_Texture->BindMultisample(m_FrameBuffer->GetColorAttachmentRendererID());
        m_QuadVA->Bind();
        Kaesar::Renderer::Submit(m_QuadVA);
        m_Texture->UnbindMultisample();
        m_QuadShader->Unbind();
    }

    virtual void OnImGuiRender() override
    {
        static bool open = true;
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &open, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu(u8"文件"))
            {
                if (ImGui::MenuItem(u8"退出"))
                {
                    Kaesar::Application::Get().CloseApp();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }


        ImGui::Begin(u8"测试");
        ImGui::ColorEdit3(u8"颜色", glm::value_ptr(m_CubeColor));
        ImGui::End();

        /// ====================== viewport ========================
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
        ImGui::Begin(u8"视口");

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        
        Kaesar::Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewportFocused || !m_ViewportHovered); // 当视口没有被激活时，不接受事件

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

        m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
        uint64_t textureID = m_PostProcessingFB->GetColorAttachmentRendererID();
        ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        ImGui::End();

        ImGui::PopStyleVar();
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
    std::shared_ptr<Kaesar::FrameBuffer> m_FrameBuffer, m_PostProcessingFB;

    std::shared_ptr<Kaesar::Texture2D> m_Texture;

    std::shared_ptr<Kaesar::Model> m_Model;

    std::shared_ptr<Kaesar::Shader> m_Shader, m_QuadShader;

    std::shared_ptr<Kaesar::PerspectiveCamera> m_Camera;

    glm::vec2 m_ViewportSize = { 200.0f, 200.0f };
    glm::vec3 m_CubeColor = { 1.0f, 1.0f, 1.0f };

    bool m_ViewportFocused = false; // 标记视口是否被聚焦
    bool m_ViewportHovered = false; // 标记鼠标是否在视口上
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