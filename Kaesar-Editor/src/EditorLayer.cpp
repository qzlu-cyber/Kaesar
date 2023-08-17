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
    }

    EditorLayer::~EditorLayer()
    {
        
    }

    void EditorLayer::OnAttach()
    {
        auto& app = Application::Get();

        m_ActiveScene = std::make_shared<Scene>();
        m_ScenePanel = std::make_shared<ScenePanel>(m_ActiveScene);

        m_ActiveScene->m_Camera->SetViewportSize((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

        FramebufferSpecification fspc;
        fspc.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::DEPTH24STENCIL8 };
        fspc.Width = 1920;
        fspc.Height = 1080;
        fspc.Samples = 4;

        m_ViewportSize = { fspc.Width, fspc.Height };
    }

    void EditorLayer::OnDetach()
    {
    }

    void EditorLayer::OnUpdate(const Timestep& timestep)
    {
        if (FramebufferSpecification spec = m_ActiveScene->GetSpec(); 
            m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
            (spec.Width != (uint32_t)m_ViewportSize.x || spec.Height != (uint32_t)m_ViewportSize.y))
        {
            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        }

        m_ActiveScene->OnUpdateEditor(timestep);
        m_SelectedEntity = m_ScenePanel->GetSelectedContext();

        if (m_ViewportFocused) // ֻ�д��ھ۽�ʱ�Ÿ������
        {
            m_ActiveScene->m_Camera->OnUpdate(timestep);
        }
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
                if (ImGui::MenuItem(u8"�½�����", "Ctrl+N"))
                {
                    NewScene();
                }
                if (ImGui::MenuItem(u8"�򿪳���", "Ctrl+O"))
                {
                    OpenScene();
                }
                if (ImGui::MenuItem(u8"���泡��", "Ctrl+Shift+S"))
                {
                    SaveSceneAs();
                }
                if (ImGui::MenuItem(u8"�˳�", "ESC"))
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
                        m_ActiveScene->CreateEntity(m_SelectedEntity);
                    }
                    else
                    {
                        KR_CORE_ERROR("��ѡ��Ҫ���Ƶ�ʵ�壡");
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

        /// ====================== Scene Setting ========================
        SceneRenderer::OnImGuiUpdate();

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
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        const ImGuiID id = window->GetID(u8"�ӿ�");

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
        uint64_t textureID = m_ActiveScene->GetMainTextureID();
        ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        // Gizmos
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, 
                    m_ViewportBounds[1].x - m_ViewportBounds[0].x, 
                   m_ViewportBounds[1].y - m_ViewportBounds[0].y);

        const glm::mat4& cameraProjection = m_ActiveScene->m_Camera->GetProjection();
        glm::mat4 cameraView = m_ActiveScene->m_Camera->GetViewMatrix();

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
            float fov = m_ActiveScene->m_Camera->GetFOV();
            if (ImGui::SliderFloat(u8"��Ұ", &fov, 10, 180)) {
                m_ActiveScene->m_Camera->SetFov(fov);
            }
            ImGui::Separator();
            //near-far clip
            float nearClip = m_ActiveScene->m_Camera->GetNear();
            float farClip = m_ActiveScene->m_Camera->GetFar();
            if (ImGui::SliderFloat(u8"Զƽ��", &farClip, 10, 10000)) {
                m_ActiveScene->m_Camera->SetFarClip(farClip);
            }
            ImGui::Separator();
            if (ImGui::SliderFloat(u8"��ƽ��", &nearClip, 0.0001, 1)) {
                m_ActiveScene->m_Camera->SetNearClip(nearClip);
            }
            ImGui::End();
        }
    }

    void EditorLayer::OnEvent(Event& event)
    {
        m_ActiveScene->m_Camera->OnEvent(event);
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(KR_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<MouseButtonPressedEvent>(KR_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
    {
        if (e.GetRepeatCount() > 0)
            return false;

        bool control = Input::IsKeyPressed(KR_KEY_LEFT_CONTROL) || Input::IsKeyPressed(KR_KEY_RIGHT_CONTROL);
        bool shift = Input::IsKeyPressed(KR_KEY_LEFT_SHIFT) || Input::IsKeyPressed(KR_KEY_RIGHT_SHIFT);

        switch (e.GetKeyCode())
        {
            case KR_KEY_N: if (control) NewScene(); break;
            case KR_KEY_O: if (control) OpenScene(); break;
            case KR_KEY_S: if (control && shift) SaveSceneAs(); break;
            // Gizmos
            case KR_KEY_Q: if (!ImGuizmo::IsUsing()) m_GizmoType = -1; break;
            case KR_KEY_W: if (!ImGuizmo::IsUsing()) m_GizmoType = ImGuizmo::OPERATION::TRANSLATE; break;
            case KR_KEY_E: if (!ImGuizmo::IsUsing()) m_GizmoType = ImGuizmo::OPERATION::ROTATE; break;
            case KR_KEY_R: if (!ImGuizmo::IsUsing()) m_GizmoType = ImGuizmo::OPERATION::SCALE; break;
            // �۽�ʵ��
            case KR_KEY_F: if (m_ScenePanel->GetSelectedContext())
                m_ActiveScene->m_Camera->SetFocalPoint(m_ScenePanel->GetSelectedContext().GetComponent<TransformComponent>().Translation);
                break;
            case KR_KEY_ESCAPE: Application::Get().CloseApp(); break;
        }

        return false;
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
        auto altIsDown = Input::IsKeyPressed(KR_KEY_LEFT_ALT) || Input::IsKeyPressed(KR_KEY_RIGHT_ALT);
        if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y && !altIsDown)
        {
            auto mouseFB = m_ActiveScene->GetMouseFB();
            mouseFB->Bind();

            int pixelData = mouseFB->ReadPixel(0, mouseX, mouseY);
            if (pixelData != -1) 
            {
                m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
                m_ScenePanel->SetSelectedEntity(m_ActiveScene->FindEntity(pixelData));
            }
            else
            {
                if (!ImGuizmo::IsOver()) 
                {
                    m_ScenePanel->SetSelectedEntity({});
                }
            }

            KR_CORE_WARN("pixel data: {0}", pixelData);

            mouseFB->Unbind();
        }

        return false;
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

            Application::Get().GetWindow().SetWindowTitle("Kaesar Rendering Engine - " + m_ActiveScene->m_Name + " scene");
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
