#include "krpch.h"
#include "SceneRenderer.h"

#include "Kaesar/Scene/Scene.h"
#include "Kaesar/Renderer/RenderCommand.h"
#include "Kaesar/Renderer/Renderer.h"

#include <glad/glad.h>

namespace Kaesar
{
    SceneRenderer::SceneData* SceneRenderer::s_Data = new SceneRenderer::SceneData;

    void SceneRenderer::Initialize()
    {
        FramebufferSpecification fspc;
        fspc.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::DEPTH24STENCIL8 };
        fspc.Width = 1920;
        fspc.Height = 1080;
        fspc.Samples = 4;
        s_Data->mainFB = FrameBuffer::Create(fspc);
        fspc.Samples = 1;
        s_Data->postProcessFB = FrameBuffer::Create(fspc);
        fspc.Attachments = { FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::DEPTH24STENCIL8 };
        s_Data->mouseFB = FrameBuffer::Create(fspc);

        float quad[] = {
            // positions   // texCoords
           -1.0f,  1.0f,  0.0f, 1.0f,
           -1.0f, -1.0f,  0.0f, 0.0f,
            1.0f, -1.0f,  1.0f, 0.0f,

           -1.0f,  1.0f,  0.0f, 1.0f,
            1.0f, -1.0f,  1.0f, 0.0f,
            1.0f,  1.0f,  1.0f, 1.0f
        };

        unsigned int quadIndices[] = {
            0, 1, 2, // first triangle
            3, 4, 5  // second triangle
        };

        BufferLayout quadLayout = {
            {ShaderDataType::Float2,"a_Position"},
            {ShaderDataType::Float2,"a_TexCoords"},
        };

        s_Data->vertexArray = VertexArray::Create();
        
        std::shared_ptr<VertexBuffer> vertexBuffer = VertexBuffer::Create(quad, sizeof(quad));
        vertexBuffer->SetLayout(quadLayout);
        s_Data->vertexArray->AddVertexBuffer(vertexBuffer);

        std::shared_ptr<IndexBuffer> indexBuffer = IndexBuffer::Create(quadIndices, sizeof(quadIndices) / sizeof(uint32_t));
        s_Data->vertexArray->SetIndexBuffer(indexBuffer);

        s_Data->cameraUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4), 0);

        if (!s_Data->basicShader)
        {
            s_Data->shaders.Load("assets/shaders/basic.glsl");
            s_Data->shaders.Load("assets/shaders/quad.glsl");
            s_Data->shaders.Load("assets/shaders/mouse.glsl");
        }
        s_Data->basicShader = s_Data->shaders.Get("basic");
        s_Data->mouseShader = s_Data->shaders.Get("mouse");
        s_Data->quadShader = s_Data->shaders.Get("quad");

        s_Data->clearColor = glm::vec3(0.196f, 0.196f, 0.196f);
    }

    void SceneRenderer::BeginScene(const PerspectiveCamera& camera)
    {   
        s_Data->cameraBuffer.ViewProjection = camera.GetViewProjection();
        s_Data->cameraBuffer.Position = camera.GetPosition();

        s_Data->cameraUniformBuffer->SetData(&s_Data->cameraBuffer, sizeof(glm::mat4), 0);

        s_Data->mainFB->Bind();

        RenderCommand::SetClearColor(glm::vec4(s_Data->clearColor, 1.0f));
        RenderCommand::Clear();
        RenderCommand::EnableDepthTest();
        Renderer::BeginScene();
    }

    void SceneRenderer::RenderScene(Scene& scene)
    {
        auto view = scene.m_Registry.view<TransformComponent, MeshComponent>();

        s_Data->mainFB->Bind();
        for (auto entity : view)
        {
            auto& transformComponent = view.get<TransformComponent>(entity);
            auto& meshComponent = view.get<MeshComponent>(entity);
            
            RenderEntityColor(entity, transformComponent, meshComponent);
        }
        s_Data->mainFB->Unbind();

        s_Data->mouseFB->Bind();
        RenderCommand::SetClearColor(glm::vec4(s_Data->clearColor, 1.0f));
        RenderCommand::Clear();
        s_Data->mouseFB->ClearAttachment(0, -1);
        for (auto entity : view)
        {
            auto& transformComponent = view.get<TransformComponent>(entity);
            auto& meshComponent = view.get<MeshComponent>(entity);

            RenderEntityID(entity, transformComponent, meshComponent);
        }
        s_Data->mouseFB->Unbind();
    }

    void SceneRenderer::RenderEntityColor(const entt::entity& entity, TransformComponent& transform, MeshComponent& mesh)
    {
        s_Data->basicShader->Bind(); // glUseProgram
        s_Data->basicShader->SetMat4("u_Model", transform.GetTransform());
        Renderer::Submit(mesh.model, s_Data->basicShader);
    }

    void SceneRenderer::RenderEntityID(const entt::entity& entity, TransformComponent& transform, MeshComponent& mesh)
    {
        s_Data->mouseShader->Bind();
        s_Data->mouseShader->SetMat4("u_Model", transform.GetTransform());
        s_Data->mouseShader->SetInt("u_ID", (uint32_t)entity);
        Renderer::Submit(mesh.model, s_Data->mouseShader);
    }

    void SceneRenderer::EndScene()
    {
        s_Data->postProcessFB->Bind();
        RenderCommand::SetState(RenderState::SRGB, true);
        RenderCommand::Clear();
        RenderCommand::DisableDepthTest();

        s_Data->quadShader->Bind();
        Texture2D::BindTexture(s_Data->mainFB->GetColorAttachmentRendererID(), 0);
        Renderer::Submit(s_Data->vertexArray, s_Data->quadShader);
        RenderCommand::SetState(RenderState::SRGB, false);
        s_Data->postProcessFB->Unbind();
        Renderer::EndScene();
    }

    void SceneRenderer::OnViewportResize(uint32_t width, uint32_t height)
    {
        s_Data->mainFB->Resize(width, height);
        s_Data->mouseFB->Resize(width, height);
        s_Data->postProcessFB->Resize(width, height);
    }
}