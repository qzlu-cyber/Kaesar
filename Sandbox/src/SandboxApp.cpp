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
<<<<<<< HEAD
=======
        Kaesar::FramebufferSpecification spec = m_FrameBuffer->GetSpecification();
        if ( m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
            (spec.Width != (uint32_t)m_ViewportSize.x || spec.Height != (uint32_t)m_ViewportSize.y))
        {
            m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_PostProcessingFB->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_Camera->SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        }

        if (Kaesar::Input::IsKeyPressed(KR_KEY_LEFT_ALT))
            KR_TRACE("Alt key is pressed (poll)!");

        m_ActiveScene->OnUpdateEditor(timestep, m_Camera);

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
>>>>>>> ab8935901c33bde322c83a952e3538ac685b4eff
        
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