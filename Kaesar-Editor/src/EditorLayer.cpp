#include "krpch.h"
#include "EditorLayer.h"

#include "imgui/imgui.h"
#include "ImGuizmo.h"

#include "Kaesar/Scene/SceneSerializer.h"
#include "Kaesar/Utils/PlatformUtils.h"
#include "Kaesar/Utils/Math.h"

#include <glad/glad.h>

namespace Kaesar {
    EditorLayer::EditorLayer()
        :Layer("Editor Layer")
    {
        RenderCommand::Init();
        m_Info = RenderCommand::Info();
        m_Camera = std::make_shared<PerspectiveCamera>(45.0f, 1.778f, 0.1f, 100.0f);
    }

    EditorLayer::~EditorLayer()
    {

    }

    void EditorLayer::OnAttach()
    {
        auto& app = Application::Get();
        m_Camera->SetViewportSize((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

        m_ActiveScene = std::make_shared<Scene>();
        m_ScenePanel = std::make_shared<ScenePanel>(m_ActiveScene);

        FramebufferSpecification fspc;
        fspc.Attachments = { FramebufferTextureFormat::RGBA8 , FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::DEPTH24STENCIL8 };
        fspc.Width = 1920;
        fspc.Height = 1080;
        fspc.Samples = 4;
        m_FrameBuffer = FrameBuffer::Create(fspc);
        fspc.Samples = 1;
        m_PostProcessingFB = FrameBuffer::Create(fspc);

        fspc.Attachments = { FramebufferTextureFormat::RED_INTEGER , FramebufferTextureFormat::DEPTH24STENCIL8 };
        m_MousePickFB = FrameBuffer::Create(fspc);

        m_ViewportSize = { fspc.Width, fspc.Height };

        float quad[] = {
            // positions   // texCoords
           -1.0f,  1.0f,  0.0f, 1.0f,
           -1.0f, -1.0f,  0.0f, 0.0f,
            1.0f, -1.0f,  1.0f, 0.0f,

           -1.0f,  1.0f,  0.0f, 1.0f,
            1.0f, -1.0f,  1.0f, 0.0f,
            1.0f,  1.0f,  1.0f, 1.0f
        };
        m_QuadVA = VertexArray::Create();
        m_QuadVB = VertexBuffer::Create(quad, sizeof(quad));

        BufferLayout quadLayout = {
            {ShaderDataType::Float2,"a_Position"},
            {ShaderDataType::Float2,"a_TexCoords"},
        };

        m_QuadVB->SetLayout(quadLayout);
        m_QuadVA->AddVertexBuffer(m_QuadVB);

        unsigned int quadIndices[] = {
            0, 1, 2, // first triangle
            3, 4, 5  // second triangle
        };

        m_QuadIB = IndexBuffer::Create(quadIndices, sizeof(quadIndices) / sizeof(uint32_t));
        m_QuadVA->SetIndexBuffer(m_QuadIB);

        m_Shaders.Load("assets/shaders/basic.glsl");
        m_Shaders.Load("assets/shaders/quad.glsl");
        m_Shaders.Load("assets/shaders/mouse.glsl");

        m_Model = std::make_shared<Model>("assets/models/spot/spot.obj");

        m_Texture = Texture2D::Create("assets/models/spot/spot_texture.png", 0);

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
            m_MousePickFB->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_Camera->SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        }

        if (Input::IsKeyPressed(KR_KEY_LEFT_ALT))
            KR_TRACE("Alt key is pressed (poll)!");

        m_ActiveScene->OnUpdateEditor(timestep, m_Camera);
        m_SelectedEntity = m_ScenePanel->GetSelectedContext();

        if (m_ViewportFocused) // ֻ�д��ھ۽�ʱ�Ÿ������
        {
            m_Camera->OnUpdate(timestep);
        }

        m_FrameBuffer->Bind();

        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        RenderCommand::Clear();
        m_FrameBuffer->ClearAttachment(1, -1);
        RenderCommand::EnableDepthTest();

        Renderer::BeginScene();

        RenderCommand::SetClearColor(glm::vec4(m_ClearColor, 1.0f));
        RenderCommand::Clear();
        m_FrameBuffer->ClearAttachment(1, -1);
        glEnable(GL_DEPTH_TEST);

        glm::mat4 model = glm::mat4(1.0f);
        if (m_SelectedEntity)
        {
            KR_CORE_TRACE("��ǰѡ�е�ʵ��Ϊ��{0}", (uint32_t)m_SelectedEntity);
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


        auto basicShader = m_Shaders.Get("basic");
        basicShader->Bind();
        m_Texture->Bind();
        basicShader->Bind(); // glUseProgram
        basicShader->SetMat4("u_Model", model);
        basicShader->SetMat4("u_ViewProjection", m_Camera->GetViewProjection());
        Renderer::Submit(m_Model);
        Renderer::EndScene();

        m_MousePickFB->Bind();
        RenderCommand::SetClearColor(glm::vec4(m_ClearColor, 1.0f));
        RenderCommand::Clear();
        m_MousePickFB->ClearAttachment(0, -1);
        glEnable(GL_DEPTH_TEST);

        m_PostProcessingFB->Bind();
        RenderCommand::Clear();
        m_PostProcessingFB->ClearAttachment(1, -1);
        glDisable(GL_DEPTH_TEST);

        auto postProcShader = m_Shaders.Get("quad");
        postProcShader->Bind();
        m_Texture->Active(0);
        m_Texture->BindMultisample(m_FrameBuffer->GetColorAttachmentRendererID(0));
        m_Texture->Active(1);
        m_Texture->BindMultisample(m_FrameBuffer->GetColorAttachmentRendererID(1));
        Renderer::Submit(m_QuadVA);

        m_PostProcessingFB->Unbind();
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
        float minWinSizeX = style.WindowMinSize.x; // ���浱ǰ��ʽ�д��ڵ���С���
        style.WindowMinSize.x = 370.0f; // ���ڵ���С���
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }
        style.WindowMinSize.x = minWinSizeX; // �� DockSpace �����󣬽���ʽ�д��ڵ���С��Ȼ�ԭ��֮ǰ�����ֵ

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu(u8"�ļ�"))
            {
                if (ImGui::MenuItem(u8"�½�����"))
                {
                    NewScene();
                }
                if (ImGui::MenuItem(u8"�򿪳���"))
                {
                    OpenScene();
                }
                if (ImGui::MenuItem(u8"���泡��"))
                {
                    SaveSceneAs();
                }
                if (ImGui::MenuItem(u8"�˳�"))
                {
                    Application::Get().CloseApp();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu(u8"�༭")) 
            {
                if (ImGui::MenuItem(u8"�½�"))
                {
                    if (m_SelectedEntity)
                    {
                        m_ActiveScene->CreateEntity();
                    }
                }
                if (ImGui::MenuItem(u8"����"))
                {
                    if (m_SelectedEntity)
                    {
                        m_ActiveScene->DuplicateEntity(m_SelectedEntity);
                    }
                    else
                    {
                        KR_CORE_ERROR("��ѡ��Ҫ���Ƶ�ʵ�壡");
                    }
                }
                //? �� bug
                if (ImGui::MenuItem(u8"ɾ��"))
                {
                    if (m_SelectedEntity)
                    {
                        m_ActiveScene->DestroyEntity(m_SelectedEntity);
                        m_SelectedEntity = {};
                    }
                    else
                    {
                        KR_CORE_ERROR("��ѡ��Ҫɾ����ʵ�壡");
                    }
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu(u8"ʵ��"))
            {
                if (ImGui::MenuItem(u8"�½�"))
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
        ImGui::Begin(u8"�ӿ�");

        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        auto viewportOffset = ImGui::GetWindowPos();
        m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();

        Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewportFocused || !m_ViewportHovered); // ���ӿ�û�б�����ʱ���������¼�

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

        m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
        uint64_t textureID = m_PostProcessingFB->GetColorAttachmentRendererID();
        ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        // Gizmos
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, 
                    m_ViewportBounds[1].x - m_ViewportBounds[0].x, 
                   m_ViewportBounds[1].y - m_ViewportBounds[0].y);

        const glm::mat4& cameraProjection = m_Camera->GetProjection();
        glm::mat4 cameraView = m_Camera->GetViewMatrix();
        ImGuizmo::DrawGrid(glm::value_ptr(cameraView), 
                           glm::value_ptr(cameraProjection),
                     glm::value_ptr(glm::mat4(1.0f)), 7);

        Entity selectedEntity = m_ScenePanel->GetSelectedContext();
        if (selectedEntity && m_GizmoType != -1)
        {
            // Entity transform
            auto& transformComponent = selectedEntity.GetComponent<TransformComponent>();
            glm::mat4 transform = transformComponent.GetTransform();

            // Snapping
            bool snap = Input::IsKeyPressed(KR_KEY_LEFT_CONTROL);
            float snapValue = 0.5f; // Snap to 0.5m for translation/scale
            // Snap to 45 degrees for rotation
            if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
                snapValue = 45.0f;

            float snapValues[3] = { snapValue, snapValue, snapValue };

            ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
                nullptr, snap ? snapValues : nullptr);

            if (ImGuizmo::IsUsing())
            {
                glm::vec3 translation, rotation, scale;
                Math::DecomposeTransform(transform, translation, rotation, scale);

                glm::vec3 deltaRotation = rotation - transformComponent.Rotation;
                transformComponent.Translation = translation;
                transformComponent.Rotation += deltaRotation;
                transformComponent.Scale = scale;
            }
        }

        ImGui::End();

        ImGui::PopStyleVar();
        ImGui::End();

        /// ======================== editor camera ==========================
        static bool camerSettings = true;
        if (camerSettings) {
            ImGui::Begin(u8"���", &camerSettings);
            //Fov
            float fov = m_Camera->GetFOV();
            if (ImGui::SliderFloat(u8"��Ұ", &fov, 10, 180)) {
                m_Camera->SetFov(fov);
            }
            ImGui::Separator();
            //near-far clip
            float nearClip = m_Camera->GetNear();
            float farClip = m_Camera->GetFar();
            if (ImGui::SliderFloat(u8"Զƽ��", &farClip, 10, 10000)) {
                m_Camera->SetFarClip(farClip);
            }
            ImGui::Separator();
            if (ImGui::SliderFloat(u8"��ƽ��", &nearClip, 0.0001, 1)) {
                m_Camera->SetNearClip(nearClip);
            }
            ImGui::End();
        }
    }

    void EditorLayer::OnEvent(Event& event)
    {
        m_Camera->OnEvent(event);
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<MouseButtonPressedEvent>(KR_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
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
            serializer.Deserializer(*filepath); // �����û�ȡ�洢�� std::optional<std::string> �����е�ʵ���ַ���ֵ
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

    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
    {
        auto [mx, my] = ImGui::GetMousePos();
        mx -= m_ViewportBounds[0].x;
        my -= m_ViewportBounds[0].y;
        glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
        my = viewportSize.y - my;
        int mouseX = (int)mx;
        int mouseY = (int)my;

        if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
        {
            m_MousePickFB->Bind();
            int pixelData = m_MousePickFB->ReadPixel(0, mouseX, mouseY);
            if (pixelData != -1) 
            {
                m_ScenePanel->SetSelectedEntity(m_ActiveScene->FindEntity(pixelData));
            }
            else
            {
                m_ScenePanel->SetSelectedEntity({});
            }
            KR_CORE_WARN("pixel data: {0}", pixelData);
            m_MousePickFB->Unbind();
        }

        return false;
    }
}
