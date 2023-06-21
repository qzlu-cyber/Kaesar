#include "hzpch.h"
#include "Application.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Log.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Hazel {
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application* Application::s_Instance = nullptr;

    Application::Application()
    {
        HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent)); // 设置回调函数

        m_ImGuiLayer = new ImGuiLayer();
        PushOverlay(m_ImGuiLayer);

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

        m_VertexArray.reset(VertexArray::Create());

        // 将顶点数组复制到一个顶点缓冲中
        std::shared_ptr<VertexBuffer> vertexBuffer;
        vertexBuffer.reset(VertexBuffer::Create(cubeVertices, sizeof(cubeVertices)));

        // 将索引数组到一个索引缓冲中
        std::shared_ptr<IndexBuffer> indexBuffer;
        indexBuffer.reset(IndexBuffer::Create(cubeIndices, sizeof(cubeIndices) / sizeof(uint32_t)));

        BufferLayout layout = {
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float3, "a_Color" }
        };

        vertexBuffer->SetLayout(layout);

        m_VertexArray->AddVertexBuffer(vertexBuffer);
        m_VertexArray->SetIndexBuffer(indexBuffer);

        const std::string basicShaderPath = "D:\\CPP\\Hazel\\Hazel\\src\\res\\shaders\\basic.shader";
        m_Shader.reset(new Shader(basicShaderPath));

        float squareVertices[3 * 4] = {
            -0.75f, -0.75f, 0.0f,
             0.75f, -0.75f, 0.0f,
             0.75f,  0.75f, 0.0f,
            -0.75f,  0.75f, 0.0f
        };

        uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

        m_SquareVA.reset(VertexArray::Create());

        std::shared_ptr<VertexBuffer> squareVB;
        squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
        squareVB->SetLayout({
            { ShaderDataType::Float3, "a_Position" }
            });
        m_SquareVA->AddVertexBuffer(squareVB);

        std::shared_ptr<IndexBuffer> squareIB;
        squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
        m_SquareVA->SetIndexBuffer(squareIB);

        const std::string blueShaderPath = "D:\\CPP\\Hazel\\Hazel\\src\\res\\shaders\\blue.shader";
        m_BlueShader.reset(new Shader(blueShaderPath));
    }

    Application::~Application()
    {
    }

    void Application::PushLayer(Layer* layer)
    {
        m_LayerStack.PushLayer(layer);
        layer->OnAttach();
    }

    void Application::PushOverlay(Layer* overlay)
    {
        m_LayerStack.PushOverlay(overlay);
        overlay->OnAttach();
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose)); // 检查事件 e 的 EventType 是否是 WindowClose，如果是就执行 OnWindowClose 函数

        // 当函数触发时，反向遍历整个 LayerStack，依次查看事件是否被这个 Layer 所响应，
        // 如果在这个过程中，事件被 handle 了，那么就停止往下层 layer 传递，否则继续遍历。
        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
        {
            (*--it)->OnEvent(e);
            if (e.m_Handled)
                break;
        }
    }

    void Application::Run()
    {
        m_Camera = std::make_shared<OrthographicCamera>();

        while (m_Running) {
            RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
            RenderCommand::Clear();
            RenderCommand::DepthTest();

            Renderer::BeginScene();

            m_Camera->CameraFreeMove();

            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            glm::mat4 view = glm::mat4(1.0f);
            glm::mat4 projection = glm::mat4(1.0f);
            model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.3f, 0.5f));
            view = m_Camera->GetViewMatrix();
            projection = glm::perspective(glm::radians(m_Camera->GetFOV()), (float)1280 / (float)720, 0.1f, 100.0f);

            m_BlueShader->Bind();

            m_BlueShader->SetMatrix("model", model);
            m_BlueShader->SetMatrix("view", view);
            m_BlueShader->SetMatrix("projection", projection);

            Renderer::Submit(m_SquareVA);

            m_Shader->Bind();

            m_Shader->SetMatrix("model", model);
            m_Shader->SetMatrix("view", view);
            m_Shader->SetMatrix("projection", projection);

            Renderer::Submit(m_VertexArray);

            Renderer::EndScene();

            for (Layer* layer : m_LayerStack)
                layer->OnUpdate();

            m_ImGuiLayer->Begin();
            for (Layer* layer : m_LayerStack)
                layer->OnImGuiRender();
            m_ImGuiLayer->End();

            m_Window->OnUpdate();
        }
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        m_Running = false;
        return true;
    }
}