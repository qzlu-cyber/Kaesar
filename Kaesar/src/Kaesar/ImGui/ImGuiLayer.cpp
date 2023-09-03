#include "krpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "ImGuizmo.h"

#include "Kaesar/Core/Application.h"
#include "Kaesar/ImGui/IconsFontAwesome5.h"

// TEMPORARY
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Kaesar {
    ImGuiLayer::ImGuiLayer()
        : Layer("ImGuiLayer")
    {
    }

    ImGuiLayer::~ImGuiLayer()
    {
    }

    void ImGuiLayer::OnAttach()
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

        ImFontConfig cfg;
        cfg.OversampleH = cfg.OversampleV = 1;

        io.Fonts->AddFontFromFileTTF("assets/fonts/msyh.ttc", 20.0f, &cfg, io.Fonts->GetGlyphRangesChineseFull());
        io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/msyh.ttc", 20.0f, &cfg, io.Fonts->GetGlyphRangesChineseFull());
        io.Fonts->Build();

        // merge in icons from Font Awesome
        static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        icons_config.GlyphMinAdvanceX = 20.0f;
        io.Fonts->AddFontFromFileTTF("assets/fonts/fa-solid-900.ttf", 20.0f * 2.0f / 3.0f, &icons_config, icons_ranges);
        
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        ImGui::StyleColorsClassic();
        SetDarkThemeColors();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        Application& app = Application::Get();
        GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 460");
    }

    void ImGuiLayer::OnDetach()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::Begin()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
    }

    void ImGuiLayer::End()
    {
        ImGuiIO& io = ImGui::GetIO();
        Application& app = Application::Get();
        io.DisplaySize = ImVec2(static_cast<float>(app.GetWindow().GetWidth()), static_cast<float>(app.GetWindow().GetHeight()));

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void ImGuiLayer::SetDarkThemeColors()
    {
        auto& colors = ImGui::GetStyle().Colors;

        auto& style = ImGui::GetStyle();
        style.WindowRounding = { 5 };
        style.ChildRounding = { 5 };
        style.FrameRounding = { 6 };
        style.GrabRounding = { 4 };

        colors[ImGuiCol_WindowBg] = ImVec4{ 0.25f, 0.25f, 0.25f, 1.000f };
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.18f, 0.21f, 0.24f, 1.00f);

        // Headers
        colors[ImGuiCol_Header] = ImVec4(0.41f, 0.40f, 0.56f, 1.00f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.43f, 0.21f, 0.48f, 1.00f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.30f, 0.10f, 0.30f, 1.00f);

        // Buttons
        colors[ImGuiCol_Button] = ImVec4(0.35f, 0.43f, 0.44f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.34f, 0.55f, 0.53f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.34f, 0.55f, 0.53f, 1.00f);

        // Frame BG
        colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
        colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
        colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.43f, 0.21f, 0.48f, 1.00f);

        // Tabs
        colors[ImGuiCol_Tab] = ImVec4(0.504f, 0.392f, 0.540f, 0.862f);
        colors[ImGuiCol_TabActive] = ImVec4(0.25f, 0.05f, 0.25f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.30f, 0.10f, 0.30f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.58f, 0.46f, 0.57f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.41f, 0.40f, 0.56f, 1.00f);

        colors[ImGuiCol_CheckMark] = ImVec4(0.34f, 0.55f, 0.53f, 1.00f);

        // Title
        colors[ImGuiCol_TitleBg] = ImVec4(0.35f, 0.43f, 0.44f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.34f, 0.55f, 0.53f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        colors[ImGuiCol_NavHighlight] = ImVec4(0.68f, 0.26f, 0.98f, 0.66f);

        colors[ImGuiCol_SliderGrab] = ImVec4{ 0.464f, 0.464f, 0.464f, 1.000f };

        colors[ImGuiCol_DockingPreview] = ImVec4(0.68f, 0.26f, 0.98f, 0.66f);
    }

    void ImGuiLayer::OnImGuiRender()
    {
        ImGui::Begin(u8"输出");
        ImGui::Text("Hello World!");
        ImGui::End();
    }

    void ImGuiLayer::OnEvent(Event& event)
    {
        if (m_BlockEvents) // 如果 ImGui 被阻塞，那么就不处理事件
        {
            ImGuiIO& io = ImGui::GetIO();
            event.m_Handled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse; // 如果事件是鼠标事件，且 ImGui 捕获了鼠标，那么就标记事件已经被处理
            event.m_Handled |= event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard; // 如果事件是键盘事件，且 ImGui 捕获了键盘，那么就标记事件已经被处理
        }
    }
}