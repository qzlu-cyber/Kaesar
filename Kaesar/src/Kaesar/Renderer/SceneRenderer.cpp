#include "krpch.h"
#include "SceneRenderer.h"

#include "Kaesar/Scene/Scene.h"
#include "Kaesar/Renderer/RenderCommand.h"
#include "Kaesar/Renderer/Renderer.h"
#include "Kaesar/Core/Application.h"
#include "Kaesar/Utils/PoissonGenerator.h"

#include <glad/glad.h>
#include "imgui.h"

namespace Kaesar
{
    SceneRenderer::SceneData* SceneRenderer::s_Data = new SceneRenderer::SceneData;

    void GeneratePoissonDisk(std::shared_ptr<Texture1D>& sampler, size_t numSamples) 
    {
        PoissonGenerator::DefaultPRNG PRNG; // 生成 Poisson-disc 分布的随机数生成器
        size_t attempts = 0; // 生成 Poisson-disc 分布的尝试次数
        // 使用 Poisson Disk Sampling 方法生成 numSamples * 2 个点，并将结果保存在 points 中
        auto points = PoissonGenerator::generatePoissonPoints(numSamples * 2, PRNG);

        // 如果生成的点数量不足 numSamples 个，则再次生成
        while (points.size() < numSamples && ++attempts < 100)
            auto points = PoissonGenerator::generatePoissonPoints(numSamples * 2, PRNG);

        // 如果尝试次数达到 100 次仍然无法生成足够的点，则报错
        if (attempts == 100)
        {
            KR_CORE_ERROR("无法生成 {0} 个 Poisson-disc 分布样本点！", numSamples);
            numSamples = points.size();
        }

        std::vector<float> data(numSamples * 2); // 用于保存样本点的数据
        for (auto i = 0, j = 0; i < numSamples; i++, j += 2)
        {
            auto& point = points[i];
            // 将样本点的 x, y 坐标保存在 data 中
            data[j] = point.x;
            data[j + 1] = point.y;
        }

        sampler = Texture1D::Create(numSamples, &data[0]);
    }

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

        // 创建阴影帧缓冲
        FramebufferSpecification shadowSpec;
        shadowSpec.Attachments = { FramebufferTextureFormat::DEPTH32 };
        shadowSpec.Width = 1024;
        shadowSpec.Height = 1024;
        shadowSpec.Samples = 1;
        s_Data->shadowFB = FrameBuffer::Create(shadowSpec);

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

        s_Data->cameraUniformBuffer = UniformBuffer::Create(sizeof(CameraData), 0); // 将和相机有关的数据绑定在 0 号绑定点
        s_Data->transformUniformBuffer = UniformBuffer::Create(sizeof(TransformData), 1); // 将和变换有关的数据绑定在 1 号绑定点
        s_Data->lightsUniformBuffer = UniformBuffer::Create(sizeof(s_Data->pointLightBuffer) + sizeof(s_Data->spotLightBuffer) + sizeof(s_Data->directionalLightBuffer), 2);
        s_Data->lightsParamsUniformBuffer = UniformBuffer::Create(sizeof(s_Data->lightsParamsBuffer), 3);
        s_Data->shadowUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4), 4);

        if (!s_Data->basicShader)
        {
            s_Data->shaders.Load("assets/shaders/basic.glsl");
            s_Data->shaders.Load("assets/shaders/quad.glsl");
            s_Data->shaders.Load("assets/shaders/mouse.glsl");
            s_Data->shaders.Load("assets/shaders/light.glsl");
            s_Data->shaders.Load("assets/shaders/depth.glsl");
        }
        s_Data->basicShader = s_Data->shaders.Get("basic");
        s_Data->mouseShader = s_Data->shaders.Get("mouse");
        s_Data->quadShader = s_Data->shaders.Get("quad");
        s_Data->lightShader = s_Data->shaders.Get("light");
        s_Data->depthShader = s_Data->shaders.Get("depth");

        s_Data->exposure = 0.2f;
        s_Data->gamma = 2.2f;
        s_Data->lightSize = 0.001f;
        
        GeneratePoissonDisk(s_Data->distributionSampler0, 32);
        GeneratePoissonDisk(s_Data->distributionSampler1, 32);

        s_Data->lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 500.0f);

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

        s_Data->lightsParamsBuffer.dirIntensity   = 0.0f;
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
                s_Data->directionalLightBuffer.ambient = light->GetAmbient() * light->GetIntensity();
                s_Data->directionalLightBuffer.diffuse = light->GetDiffuse() * light->GetIntensity();
                s_Data->directionalLightBuffer.specular = light->GetSpecular() * light->GetIntensity();
                s_Data->directionalLightBuffer.direction = light->GetDirection();

                // shadow
                s_Data->lightView = glm::lookAt(glm::vec3(s_Data->directionalLightBuffer.direction), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                s_Data->shadowBuffer.lightViewProjection = s_Data->lightProjection * s_Data->lightView;
                s_Data->shadowUniformBuffer->SetData(&s_Data->shadowBuffer, sizeof(glm::mat4));

                s_Data->lightsParamsBuffer.dirIntensity = light->GetIntensity();
            }
            if (lightComponent.type == LightType::Point)
            {
                auto light = dynamic_cast<PointLight*>(lightComponent.light.get());
                s_Data->pointLightBuffer.position = transformComponent.Translation;

                s_Data->pointLightBuffer.ambient = light->GetAmbient() * light->GetIntensity();
                s_Data->pointLightBuffer.diffuse = light->GetDiffuse() * light->GetIntensity();
                s_Data->pointLightBuffer.specular = light->GetSpecular() * light->GetIntensity();

                s_Data->lightsParamsBuffer.pointLinear = light->GetLinear();
                s_Data->lightsParamsBuffer.pointQuadratic = light->GetQuadratic();
            }
            if (lightComponent.type == LightType::Spot)
            {
                auto light = dynamic_cast<SpotLight*>(lightComponent.light.get());
                s_Data->spotLightBuffer.position = transformComponent.Translation;
                s_Data->spotLightBuffer.direction = transformComponent.Rotation;
                s_Data->spotLightBuffer.ambient = light->GetAmbient() * light->GetIntensity();
                s_Data->spotLightBuffer.diffuse = light->GetDiffuse() * light->GetIntensity();
                s_Data->spotLightBuffer.specular = light->GetSpecular() * light->GetIntensity();

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

        // 渲染深度贴图
        s_Data->shadowFB->Bind();
        RenderCommand::SetState(RenderState::DEPTH_TEST, true);
        glClear(GL_DEPTH_BUFFER_BIT);
        for (auto& entity : view)
        {
            auto& transformComponent = view.get<TransformComponent>(entity);
            auto& meshComponent = view.get<MeshComponent>(entity);

            if (!meshComponent.path.empty())
            {
                s_Data->depthShader->Bind();
                s_Data->depthShader->SetMat4("transform.u_Transform", transformComponent.GetTransform());
                SceneRenderer::RenderEntityColor(entity, transformComponent, meshComponent, s_Data->depthShader);
            }
        }
        s_Data->shadowFB->Unbind();

        s_Data->mainFB->Bind();
        RenderCommand::SetState(RenderState::DEPTH_TEST, true);
        RenderCommand::Clear();

        s_Data->basicShader->Bind();
        Texture2D::BindTexture(s_Data->shadowFB->GetDepthAttachmentRendererID(), 3);
        Texture1D::BindTexture(s_Data->distributionSampler0->GetRendererID(), 4);
        Texture1D::BindTexture(s_Data->distributionSampler1->GetRendererID(), 5);
        s_Data->basicShader->SetFloat("push.size", s_Data->lightSize);
        for (auto entity : view)
        {
            auto& transformComponent = view.get<TransformComponent>(entity);
            auto& meshComponent = view.get<MeshComponent>(entity);

            if (!meshComponent.path.empty()) // 如果有模型路径
            {
                s_Data->transformBuffer.transform = transformComponent.GetTransform();
                s_Data->transformUniformBuffer->SetData(&s_Data->transformBuffer, sizeof(TransformData), 0);

                if (scene.m_Registry.has<MaterialComponent>(entity)) // 如果有材质组件
                {
                    auto& materialComponent = scene.m_Registry.get<MaterialComponent>(entity);
                    SceneRenderer::RenderEntityColor(entity, transformComponent, meshComponent, materialComponent); // 使用它自身的材质组件渲染
                }
                else
                {
                    SceneRenderer::RenderEntityColor(entity, transformComponent, meshComponent); // 否则使用默认渲染
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
        RenderCommand::SetState(RenderState::CULL, false);
        s_Data->basicShader->Bind(); // glUseProgram
        Renderer::Submit(mesh.model, s_Data->basicShader);
        RenderCommand::SetState(RenderState::CULL, true);
    }

    void SceneRenderer::RenderEntityColor(const entt::entity& entity, TransformComponent& transform, MeshComponent& mesh, MaterialComponent& material)
    {
        RenderCommand::SetState(RenderState::CULL, false);
        Renderer::Submit(material.material, mesh.model);
        RenderCommand::SetState(RenderState::CULL, true);
    }

    void SceneRenderer::RenderEntityColor(const entt::entity& entity, TransformComponent& tc, MeshComponent& mc, const std::shared_ptr<Shader>& shader)
    {
        RenderCommand::SetState(RenderState::CULL, false);
        shader->Bind();
        Renderer::Submit(mc.model, shader);
        RenderCommand::SetState(RenderState::CULL, true);
    }

    void SceneRenderer::RenderEntityID(const entt::entity& entity, TransformComponent& transform, MeshComponent& mesh)
    {
        s_Data->mouseShader->Bind();
        Renderer::Submit(mesh.model, s_Data->mouseShader);
    }

    void SceneRenderer::EndScene()
    {
        s_Data->postProcessFB->Bind();
        RenderCommand::Clear();
        RenderCommand::DisableDepthTest();

        s_Data->quadShader->Bind();
        s_Data->quadShader->SetFloat("pc.exposure", s_Data->exposure);
        s_Data->quadShader->SetFloat("pc.gamma", s_Data->gamma);
        Texture2D::BindTexture(s_Data->mainFB->GetColorAttachmentRendererID(), 0);
        Renderer::Submit(s_Data->vertexArray, s_Data->quadShader);
        s_Data->postProcessFB->Unbind();
        Renderer::EndScene();
    }

    void SceneRenderer::OnImGuiUpdate()
    {
        /// ====================== Scene Setting ========================
        ImGui::Begin(u8"场景设置");

        ImGui::DragFloat(u8"曝   光", &s_Data->exposure, 0.001f, -2, 4);

        ImGui::DragFloat(u8"gamma值", &s_Data->gamma, 0.01f, 0, 4);

        ImGui::DragFloat(u8"光源大小", &s_Data->lightSize, 0.0001, 0, 100);

        static bool vSync = true;
        ImGui::Checkbox(u8"垂直同步", &vSync);
        Application::Get().GetWindow().SetVSync(vSync);

        //DepthMap
        static bool showDepth = false;
        if (ImGui::Button(u8"深度贴图"))
        {
            showDepth = !showDepth;
        }

        ImGui::End();

        if (showDepth) 
        {
            ImGui::Begin(u8"深度贴图");
            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
            ImGui::Image(reinterpret_cast<void*>(s_Data->shadowFB->GetDepthAttachmentRendererID()), viewportPanelSize);
            ImGui::End();
        }
    }

    void SceneRenderer::OnViewportResize(uint32_t width, uint32_t height)
    {
        s_Data->mainFB->Resize(width, height);
        s_Data->mouseFB->Resize(width, height);
        s_Data->postProcessFB->Resize(width, height);
    }
}