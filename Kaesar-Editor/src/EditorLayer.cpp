#include "krpch.h"
#include "EditorLayer.h"

#include "Kaesar/Scene/SceneSerializer.h"
#include "Kaesar/Utils/PlatformUtils.h"

#include <imgui/imgui.h>

namespace Kaesar {
    EditorLayer::EditorLayer()
        :Layer("Editor Layer")
    {
        RenderCommand::Init();
        m_Info = RenderCommand::Info();
    }

    EditorLayer::~EditorLayer()
    {

    }

    void EditorLayer::OnAttach()
    {
        auto& app = Application::Get();

        m_ActiveScene = std::make_shared<Scene>();
        m_ScenePanel = std::make_shared<ScenePanel>(m_ActiveScene);

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

        const std::string basicShaderPath = "assets/shaders/basic.glsl";
        m_Shader = Shader::Create(basicShaderPath);

        const std::string quadShaderPath = "assets/shaders/quad.glsl";
        m_QuadShader = Shader::Create(quadShaderPath);
        m_QuadShader->SetInt("u_Texture", 0);

        m_Model = std::make_shared<Model>("assets/models/spot/spot.obj");

        m_Texture = Texture2D::Create("assets/models/spot/spot_texture.png", 0);

        m_Camera = std::make_shared<PerspectiveCamera>(45.0f, 1.778f, 0.1f, 100.0f);
        m_Camera->SetViewportSize((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

        m_Entity = m_ActiveScene->CreateEntity("spot");
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
        m_SelectedEntity = m_ScenePanel->GetSelectedContext();

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
        if (m_SelectedEntity)
        {
            KR_CORE_TRACE("当前选中的实体为：{0}", (uint32_t)m_SelectedEntity);
            glm::vec3 translate = m_SelectedEntity.GetComponent<TransformComponent>().Translation;
            glm::vec3 rotate = m_SelectedEntity.GetComponent<TransformComponent>().Rotation;
            glm::vec3 scale = m_SelectedEntity.GetComponent<TransformComponent>().Scale;
            model = glm::scale(model, scale);
            model = glm::rotate(model, glm::radians(rotate.x), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(rotate.y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(rotate.z), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::translate(model, translate);
        }
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
        ImGuiStyle& style = ImGui::GetStyle();
        float minWinSizeX = style.WindowMinSize.x; // 保存当前样式中窗口的最小宽度
        style.WindowMinSize.x = 370.0f; // 窗口的最小宽度
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }
        style.WindowMinSize.x = minWinSizeX; // 在 DockSpace 结束后，将样式中窗口的最小宽度还原回之前保存的值

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu(u8"文件"))
            {
                if (ImGui::MenuItem(u8"新建场景"))
                {
                    NewScene();
                }
                if (ImGui::MenuItem(u8"打开场景"))
                {
                    OpenScene();
                }
                if (ImGui::MenuItem(u8"保存场景"))
                {
                    SaveSceneAs();
                }
                if (ImGui::MenuItem(u8"退出"))
                {
                    Application::Get().CloseApp();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu(u8"编辑")) 
            {
                if (ImGui::MenuItem(u8"新建"))
                {
                    if (m_SelectedEntity)
                    {
                        m_ActiveScene->CreateEntity();
                    }
                }
                if (ImGui::MenuItem(u8"复制"))
                {
                    if (m_SelectedEntity)
                    {
                        m_ActiveScene->DuplicateEntity(m_SelectedEntity);
                    }
                    else
                    {
                        KR_CORE_ERROR("请选择要复制的实体！");
                    }
                }
                //? 有 bug
                if (ImGui::MenuItem(u8"删除"))
                {
                    if (m_SelectedEntity)
                    {
                        m_ActiveScene->DestroyEntity(m_SelectedEntity);
                        m_SelectedEntity = {};
                    }
                    else
                    {
                        KR_CORE_ERROR("请选择要删除的实体！");
                    }
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu(u8"实体"))
            {
                if (ImGui::MenuItem(u8"新建"))
                {
                    m_ActiveScene->CreateEntity();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        m_ScenePanel->OnImGuiRender();

        /// ====================== Renderer info ========================
        ImGui::Begin("Renderer info");
        ImGui::Text(m_Info.c_str());
        ImGui::Text("\nApplication average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::Text("%d vertices, %d indices (%d triangles)", io.MetricsRenderVertices, io.MetricsRenderIndices, io.MetricsRenderIndices / 3);
        ImGui::Text("%d active windows (%d visible)", io.MetricsActiveWindows, io.MetricsRenderWindows);
        ImGui::Text("%d active allocations", io.MetricsActiveAllocations);
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

        /// ======================== editor camera ==========================
        static bool camerSettings = true;
        if (camerSettings) {
            ImGui::Begin(u8"相机", &camerSettings);
            //Fov
            float fov = m_Camera->GetFOV();
            if (ImGui::SliderFloat(u8"视野", &fov, 10, 180)) {
                m_Camera->SetFov(fov);
            }
            ImGui::Separator();
            //near-far clip
            float nearClip = m_Camera->GetNear();
            float farClip = m_Camera->GetFar();
            if (ImGui::SliderFloat(u8"远平面", &farClip, 10, 10000)) {
                m_Camera->SetFarClip(farClip);
            }
            ImGui::Separator();
            if (ImGui::SliderFloat(u8"近平面", &nearClip, 0.0001, 1)) {
                m_Camera->SetNearClip(nearClip);
            }
            ImGui::End();
        }
    }

    void EditorLayer::OnEvent(Event& event)
    {
        m_Camera->OnEvent(event);
    }

    void EditorLayer::NewScene()
    {
        m_ActiveScene = std::make_shared<Scene>();
        m_ActiveScene->OnViewportResize(m_ViewportSize.x, m_ViewportSize.y);
        m_ScenePanel = std::make_shared<ScenePanel>(m_ActiveScene);
    }

    void EditorLayer::OpenScene()
    {
        std::optional<std::string> filepath = FileDialogs::OpenFile("Kaesar Scene (*.kaesar)\0*.kaesar\0");
        if (filepath)
        {
            m_ActiveScene = std::make_shared<Scene>();
            m_ActiveScene->OnViewportResize(m_ViewportSize.x, m_ViewportSize.y);
            m_ScenePanel = std::make_shared<ScenePanel>(m_ActiveScene);

            SceneSerializer serializer(m_ActiveScene);
            serializer.Deserializer(*filepath); // 解引用获取存储在 std::optional<std::string> 类型中的实际字符串值
        }  
    }

    void EditorLayer::SaveSceneAs()
    {
        std::optional<std::string> filepath = FileDialogs::SaveFile("Kaesar Scene (*.kaesar)\0*.kaesar\0");
        if (filepath)
        {
            SceneSerializer serializer(m_ActiveScene);
            serializer.Serializer(*filepath);
        }
    }
}
