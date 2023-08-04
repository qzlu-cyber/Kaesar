#include "krpch.h"
#include "EditorLayer.h"

#include "imgui/imgui.h"

namespace Kaesar {
    EditorLayer::EditorLayer()
        :Layer("Editor Layer")
    {
        RenderCommand::Init();
    }

    EditorLayer::~EditorLayer()
    {

    }

    void EditorLayer::OnAttach()
    {
        auto& app = Application::Get();

        m_ActiveScene = std::make_shared<Scene>();

        float quad[] = {
            // positions        // texture coords
            1.0f,  1.0f, 0.0f,    1.0f, 1.0f,   // top right
            1.0f, -1.0f, 0.0f,    1.0f, 0.0f,   // bottom right
           -1.0f, -1.0f, 0.0f,    0.0f, 0.0f,   // bottom left
           -1.0f,  1.0f, 0.0f,    0.0f, 1.0f    // top left 
        };

        unsigned int quadIndices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };

        m_QuadVA = VertexArray::Create();

        // 将顶点数组复制到一个顶点缓冲中
        m_QuadVB = VertexBuffer::Create(&quad[0], sizeof(quad));

        // 将索引数组到一个索引缓冲中
        m_QuadIB = IndexBuffer::Create(&quadIndices[0], sizeof(quadIndices) / sizeof(uint32_t));

        BufferLayout quadLayout = {
            { ShaderDataType::Float2, "a_Position" },
            { ShaderDataType::Float2, "a_TexCoords" }
        };

        m_QuadVB->SetLayout(quadLayout);
        m_QuadVA->AddVertexBuffer(m_QuadVB);
        m_QuadVA->SetIndexBuffer(m_QuadIB);

        FramebufferSpecification fspc;
        fspc.Width = app.GetWindow().GetWidth();
        fspc.Height = app.GetWindow().GetHeight();

        m_ViewportSize = { fspc.Width, fspc.Height };

        fspc.Samples = 4; // 4x MSAA
        m_FrameBuffer = FrameBuffer::Create(fspc); // MSAA framebuffer
        fspc.Samples = 1;
        m_PostProcessingFB = FrameBuffer::Create(fspc); // 后处理 framebuffer

        const std::string basicShaderPath = "D:\\CPP\\Kaesar\\Kaesar\\src\\res\\shaders\\basic.glsl";
        m_Shader = Shader::Create(basicShaderPath);

        const std::string quadShaderPath = "D:\\CPP\\Kaesar\\Kaesar\\src\\res\\shaders\\quad.glsl";
        m_QuadShader = Shader::Create(quadShaderPath);
        m_QuadShader->SetInt("u_Texture", 0);

        m_Model = std::make_shared<Model>("D:\\CPP\\Kaesar\\Kaesar\\src\\res\\models\\spot\\spot.obj");

        m_Texture = Texture2D::Create("D:\\CPP\\Kaesar\\Kaesar\\src\\res\\models\\spot\\spot_texture.png", 0);

        m_Camera = std::make_shared<PerspectiveCamera>(45.0f, 1.778f, 0.1f, 100.0f);
        m_Camera->SetViewportSize((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

        auto spotEntity = m_ActiveScene->CreateEntity("spot");
    }

    void EditorLayer::OnDetach()
    {
    }

    void EditorLayer::OnUpdate(const Timestep& timestep)
    {
        FramebufferSpecification spec = m_FrameBuffer->GetSpecification();
        if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
            (spec.Width != (uint32_t)m_ViewportSize.x || spec.Height != (uint32_t)m_ViewportSize.y))
        {
            m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_PostProcessingFB->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_Camera->SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        }

        if (Input::IsKeyPressed(KR_KEY_LEFT_ALT))
            KR_TRACE("Alt key is pressed (poll)!");

        m_ActiveScene->OnUpdateEditor(timestep, m_Camera);

        m_FrameBuffer->Bind();

        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        RenderCommand::Clear();
        RenderCommand::EnableDepthTest();

        if (m_ViewportFocused) // 只有窗口聚焦时才更新相机
        {
            m_Camera->OnUpdate(timestep);
        }

        Renderer::BeginScene();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(140.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));

        /// ====================== spot ========================
        m_Texture->Active(0);
        m_Texture->Bind();

        m_Shader->Bind(); // glUseProgram
        m_Shader->SetMat4("u_Model", model);
        m_Shader->SetMat4("u_ViewProjection", m_Camera->GetViewProjection());

        Renderer::Submit(m_Model);
        /// ====================== spot end =====================
        m_Texture->Unbind();
        m_Shader->Unbind();
        m_FrameBuffer->Unbind();

        Renderer::EndScene();

        m_FrameBuffer->BlitMultiSample(m_FrameBuffer->GetRendererID(), m_PostProcessingFB->GetRendererID());

        m_FrameBuffer->Unbind();
        RenderCommand::SetClearColor({ 1.0f, 1.0f, 1.0f, 1.0f });
        RenderCommand::ClearColor();
        RenderCommand::DisableDepthTest();

        m_QuadShader->Bind();
        m_Texture->BindMultisample(m_FrameBuffer->GetColorAttachmentRendererID());
        m_QuadVA->Bind();
        Renderer::Submit(m_QuadVA);
        m_Texture->UnbindMultisample();
        m_QuadShader->Unbind();
    }

    void EditorLayer::OnImGuiRender()
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
                    Application::Get().CloseApp();
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

        Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewportFocused || !m_ViewportHovered); // 当视口没有被激活时，不接受事件

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

        m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
        uint64_t textureID = m_PostProcessingFB->GetColorAttachmentRendererID();
        ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        ImGui::End();

        ImGui::PopStyleVar();
        ImGui::End();
    }

    void EditorLayer::OnEvent(Event& event)
    {
        if (event.GetEventType() == EventType::KeyPressed)
        {
            KeyPressedEvent& e = static_cast<KeyPressedEvent&>(event);
            if (e.GetKeyCode() == KR_KEY_LEFT_ALT)
                KR_TRACE("Alt key is pressed (event)!");
            KR_TRACE("{0}", static_cast<char>(e.GetKeyCode()));
        }
    }
}
