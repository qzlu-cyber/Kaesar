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
        fspc.Attachments = { FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::DEPTH24STENCIL8 };
        fspc.Width = 1920;
        fspc.Height = 1080;
        fspc.Samples = 4;
        s_Data->mainFB = FrameBuffer::Create(fspc);
        fspc.Attachments = { FramebufferTextureFormat::RGBA8 , FramebufferTextureFormat::DEPTH24STENCIL8 };
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

        s_Data->cameraUniformBuffer = UniformBuffer::Create(sizeof(CameraData), 0); // ��������йص����ݰ��� 0 �Ű󶨵�
        s_Data->transformUniformBuffer = UniformBuffer::Create(sizeof(TransformData), 1); // ���ͱ任�йص����ݰ��� 1 �Ű󶨵�
        s_Data->lightsUniformBuffer = UniformBuffer::Create(sizeof(s_Data->pointLightBuffer) + sizeof(s_Data->spotLightBuffer) + sizeof(s_Data->directionalLightBuffer), 2);
        s_Data->lightsParamsUniformBuffer = UniformBuffer::Create(sizeof(s_Data->lightsParamsBuffer), 3);

        if (!s_Data->basicShader)
        {
            s_Data->shaders.Load("assets/shaders/basic.glsl");
            s_Data->shaders.Load("assets/shaders/quad.glsl");
            s_Data->shaders.Load("assets/shaders/mouse.glsl");
            s_Data->shaders.Load("assets/shaders/light.glsl");
        }
        s_Data->basicShader = s_Data->shaders.Get("basic");
        s_Data->mouseShader = s_Data->shaders.Get("mouse");
        s_Data->quadShader = s_Data->shaders.Get("quad");
        s_Data->lightShader = s_Data->shaders.Get("light");

        s_Data->exposure = 0.2f;

        s_Data->clearColor = glm::vec3(0.196f, 0.196f, 0.196f);
    }

    void SceneRenderer::BeginScene(const PerspectiveCamera& camera)
    {   
        s_Data->cameraBuffer.viewProjection = camera.GetViewProjection();
        s_Data->cameraBuffer.position = camera.GetPosition();
        s_Data->cameraUniformBuffer->SetData(&s_Data->cameraBuffer, sizeof(CameraData), 0);

        s_Data->directionalLightBuffer.direction = glm::vec3(0.0f, 0.0f, 0.0f);
        s_Data->directionalLightBuffer.ambient   = glm::vec3(0.0f, 0.0f, 0.0f);
        s_Data->directionalLightBuffer.diffuse   = glm::vec3(0.0f, 0.0f, 0.0f);
        s_Data->directionalLightBuffer.specular  = glm::vec3(0.0f, 0.0f, 0.0f);

        s_Data->pointLightBuffer.position = glm::vec3(0.0f, 0.0f, 0.0f);
        s_Data->pointLightBuffer.ambient  = glm::vec3(0.0f, 0.0f, 0.0f);
        s_Data->pointLightBuffer.diffuse  = glm::vec3(0.0f, 0.0f, 0.0f);
        s_Data->pointLightBuffer.specular = glm::vec3(0.0f, 0.0f, 0.0f);

        s_Data->spotLightBuffer.position  = glm::vec3(0.0f, 0.0f, 0.0f);
        s_Data->spotLightBuffer.direction = glm::vec3(0.0f, 0.0f, 0.0f);
        s_Data->spotLightBuffer.ambient   = glm::vec3(0.0f, 0.0f, 0.0f);
        s_Data->spotLightBuffer.diffuse   = glm::vec3(0.0f, 0.0f, 0.0f);
        s_Data->spotLightBuffer.specular  = glm::vec3(0.0f, 0.0f, 0.0f);

        s_Data->lightsParamsBuffer.pointLinear    = 0.0f;
        s_Data->lightsParamsBuffer.pointQuadratic = 0.0f;
        s_Data->lightsParamsBuffer.spotLinear     = 0.0f;
        s_Data->lightsParamsBuffer.spotQuadratic  = 0.0f;
        s_Data->lightsParamsBuffer.innerCutOff    = 0.0f;
        s_Data->lightsParamsBuffer.outerCutOff    = 0.0f;

        s_Data->mainFB->Bind();

        RenderCommand::SetClearColor(glm::vec4(s_Data->clearColor, 1.0f));
        RenderCommand::Clear();
        RenderCommand::EnableDepthTest();
        Renderer::BeginScene();
    }

    void SceneRenderer::RenderScene(Scene& scene)
    {
        auto& lightView = scene.m_Registry.view<TransformComponent, LightComponent>();

        for (auto entity : lightView)
        {
            auto& transformComponent = lightView.get<TransformComponent>(entity);
            auto& lightComponent = lightView.get<LightComponent>(entity);

            if (lightComponent.type == LightType::Directional)
            {
                auto light = dynamic_cast<DirectionalLight*>(lightComponent.light.get());
                s_Data->directionalLightBuffer.ambient = light->GetAmbient();
                s_Data->directionalLightBuffer.diffuse = light->GetDiffuse();
                s_Data->directionalLightBuffer.specular = light->GetSpecular();
                s_Data->directionalLightBuffer.direction = light->GetDirection();
            }
            if (lightComponent.type == LightType::Point)
            {
                auto light = dynamic_cast<PointLight*>(lightComponent.light.get());
                s_Data->pointLightBuffer.position = transformComponent.Translation;

                s_Data->pointLightBuffer.ambient = light->GetAmbient();
                s_Data->pointLightBuffer.diffuse = light->GetDiffuse();
                s_Data->pointLightBuffer.specular = light->GetSpecular();

                s_Data->lightsParamsBuffer.pointLinear = light->GetLinear();
                s_Data->lightsParamsBuffer.pointQuadratic = light->GetQuadratic();
            }
            if (lightComponent.type == LightType::Spot)
            {
                auto light = dynamic_cast<SpotLight*>(lightComponent.light.get());
                s_Data->spotLightBuffer.position = transformComponent.Translation;
                s_Data->spotLightBuffer.direction = transformComponent.Rotation;
                s_Data->spotLightBuffer.ambient = light->GetAmbient();
                s_Data->spotLightBuffer.diffuse = light->GetDiffuse();
                s_Data->spotLightBuffer.specular = light->GetSpecular();

                s_Data->lightsParamsBuffer.spotLinear = light->GetLinear();
                s_Data->lightsParamsBuffer.spotQuadratic = light->GetQuadratic();
                s_Data->lightsParamsBuffer.innerCutOff = glm::cos(glm::radians(light->GetInnerCutOff()));
                s_Data->lightsParamsBuffer.outerCutOff = glm::cos(glm::radians(light->GetOuterCutOff()));
            }
        }

        s_Data->lightsUniformBuffer->SetData(&s_Data->pointLightBuffer, sizeof(s_Data->pointLightBuffer), 0);
        s_Data->lightsUniformBuffer->SetData(&s_Data->spotLightBuffer, sizeof(s_Data->spotLightBuffer), sizeof(s_Data->pointLightBuffer));
        s_Data->lightsUniformBuffer->SetData(&s_Data->directionalLightBuffer, sizeof(s_Data->directionalLightBuffer), sizeof(s_Data->pointLightBuffer) + sizeof(s_Data->spotLightBuffer));
        s_Data->lightsParamsUniformBuffer->SetData(&s_Data->lightsParamsBuffer, sizeof(s_Data->lightsParamsBuffer), 0);

        auto view = scene.m_Registry.view<TransformComponent, MeshComponent>();

        s_Data->mainFB->Bind();
        for (auto entity : view)
        {
            auto& transformComponent = view.get<TransformComponent>(entity);
            auto& meshComponent = view.get<MeshComponent>(entity);

            if (!meshComponent.path.empty()) // �����ģ��·��
            {
                s_Data->transformBuffer.transform = transformComponent.GetTransform();
                s_Data->transformUniformBuffer->SetData(&s_Data->transformBuffer, sizeof(TransformData), 0);

                if (scene.m_Registry.has<MaterialComponent>(entity)) // ����в������
                {
                    auto& materialComponent = scene.m_Registry.get<MaterialComponent>(entity);
                    SceneRenderer::RenderEntityColor(entity, transformComponent, meshComponent, materialComponent); // ʹ��������Ĳ��������Ⱦ
                }
                else
                {
                    SceneRenderer::RenderEntityColor(entity, transformComponent, meshComponent); // ����ʹ��Ĭ����Ⱦ
                }
            }
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

            if (!meshComponent.path.empty())
            {
                s_Data->transformBuffer.transform = transformComponent.GetTransform();
                s_Data->transformBuffer.id = (uint32_t)entity;
                s_Data->transformUniformBuffer->SetData(&s_Data->transformBuffer, sizeof(TransformData), 0);

                RenderEntityID(entity, transformComponent, meshComponent);
            }
        }
        s_Data->mouseFB->Unbind();
    }

    void SceneRenderer::RenderEntityColor(const entt::entity& entity, TransformComponent& transform, MeshComponent& mesh)
    {
        s_Data->basicShader->Bind(); // glUseProgram
        Renderer::Submit(mesh.model, s_Data->basicShader);
    }

    void SceneRenderer::RenderEntityColor(const entt::entity& entity, TransformComponent& transform, MeshComponent& mesh, MaterialComponent& material)
    {
        Renderer::Submit(material.material, mesh.model);
    }

    void SceneRenderer::RenderEntityID(const entt::entity& entity, TransformComponent& transform, MeshComponent& mesh)
    {
        s_Data->mouseShader->Bind();
        Renderer::Submit(mesh.model, s_Data->mouseShader);
    }

    void SceneRenderer::EndScene()
    {
        s_Data->postProcessFB->Bind();
        RenderCommand::SetState(RenderState::SRGB, true);
        RenderCommand::Clear();
        RenderCommand::DisableDepthTest();

        s_Data->quadShader->Bind();
        s_Data->quadShader->SetFloat("pc.exposure", s_Data->exposure);
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