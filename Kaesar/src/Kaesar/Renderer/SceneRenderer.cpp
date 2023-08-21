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
    static SceneRenderer::SceneData s_Data;

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
        ///------------------------------------------------Main Render Pass----------------------------------------///
        FramebufferSpecification mainFbSpec;
        mainFbSpec.Attachments = { FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::DEPTH24STENCIL8 };
        mainFbSpec.Width = 1920;
        mainFbSpec.Height = 1080;
        mainFbSpec.Samples = 4;
        mainFbSpec.ClearColor = glm::vec4(0.196f, 0.196f, 0.196f, 1.0f);

        RenderPassSpecification mainRenderPassSpec;
        mainRenderPassSpec.TargetFrameBuffer = FrameBuffer::Create(mainFbSpec);
        s_Data.mainPass = RenderPass::Create(mainRenderPassSpec);
        RenderCommand::SetClearColor(mainFbSpec.ClearColor);

        ///-------------------------------------------------Mouse Pick Pass---------------------------------------///
        FramebufferSpecification mouseFbSpec;
        mouseFbSpec.Attachments = { FramebufferTextureFormat::RED_INTEGER , FramebufferTextureFormat::DEPTH24STENCIL8 };
        mouseFbSpec.Width = 1920;
        mouseFbSpec.Height = 1080;
        mouseFbSpec.Samples = 1;
        mouseFbSpec.ClearColor = glm::vec4(0.196f, 0.196f, 0.196f, 1.0f);

        RenderPassSpecification mouseRenderPassSpec;
        mouseRenderPassSpec.TargetFrameBuffer = FrameBuffer::Create(mouseFbSpec);
        s_Data.mousePass = RenderPass::Create(mouseRenderPassSpec);
        RenderCommand::SetClearColor(mouseFbSpec.ClearColor);

        ///--------------------------------------------Post Processing Pass--------------------------------------///
        FramebufferSpecification postProcFB;
        postProcFB.Attachments = { FramebufferTextureFormat::RGBA8 , FramebufferTextureFormat::DEPTH24STENCIL8 };
        postProcFB.Width = 1920;
        postProcFB.Height = 1080;
        postProcFB.Samples = 1;
        postProcFB.ClearColor = glm::vec4(0.196f, 0.196f, 0.196f, 1.0f);

        RenderPassSpecification finalPassSpec;
        finalPassSpec.TargetFrameBuffer = FrameBuffer::Create(postProcFB);
        s_Data.postPass = RenderPass::Create(finalPassSpec);

        ///------------------------------------------------Shadow Pass------------------------------------------///
        // 创建阴影帧缓冲
        FramebufferSpecification shadowSpec;
        shadowSpec.Attachments = { FramebufferTextureFormat::DEPTH32 };
        shadowSpec.Width = 1024;
        shadowSpec.Height = 1024;
        shadowSpec.Samples = 1;
        shadowSpec.ClearColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        
        RenderPassSpecification shadowPassSpec;
        shadowPassSpec.TargetFrameBuffer = FrameBuffer::Create(shadowSpec);
        s_Data.shadowPass = RenderPass::Create(shadowPassSpec);

        ///-------------------------------------------------Quad-----------------------------------------------///
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

        s_Data.vertexArray = VertexArray::Create();
        
        std::shared_ptr<VertexBuffer> vertexBuffer = VertexBuffer::Create(quad, sizeof(quad));
        vertexBuffer->SetLayout(quadLayout);
        s_Data.vertexArray->AddVertexBuffer(vertexBuffer);

        std::shared_ptr<IndexBuffer> indexBuffer = IndexBuffer::Create(quadIndices, sizeof(quadIndices) / sizeof(uint32_t));
        s_Data.vertexArray->SetIndexBuffer(indexBuffer);

        ///-------------------------------------------------Shaders--------------------------------------------///
        if (!s_Data.basicShader)
        {
            s_Data.shaders.Load("assets/shaders/basic.glsl");
            s_Data.shaders.Load("assets/shaders/quad.glsl");
            s_Data.shaders.Load("assets/shaders/mouse.glsl");
            s_Data.shaders.Load("assets/shaders/light.glsl");
            s_Data.shaders.Load("assets/shaders/depth.glsl");
        }
        s_Data.basicShader = s_Data.shaders.Get("basic");
        s_Data.mouseShader = s_Data.shaders.Get("mouse");
        s_Data.quadShader = s_Data.shaders.Get("quad");
        s_Data.lightShader = s_Data.shaders.Get("light");
        s_Data.depthShader = s_Data.shaders.Get("depth");

        ///-------------------------------------------------Uniforms-------------------------------------------///
        s_Data.cameraUniformBuffer = UniformBuffer::Create(sizeof(CameraData), 0); // 将和相机有关的数据绑定在 0 号绑定点
        s_Data.transformUniformBuffer = UniformBuffer::Create(sizeof(TransformData), 1); // 将和变换有关的数据绑定在 1 号绑定点
        s_Data.lightsUniformBuffer = UniformBuffer::Create(sizeof(s_Data.pointLightsBuffer) + sizeof(s_Data.spotLightsBuffer) + sizeof(s_Data.directionalLightBuffer), 2);
        s_Data.lightsParamsUniformBuffer = UniformBuffer::Create(sizeof(s_Data.lightsParamsBuffer), 3);
        s_Data.shadowUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4), 4);

        s_Data.exposure = 0.2f;
        s_Data.gamma = 2.2f;
        s_Data.lightSize = 0.001f;
        
        GeneratePoissonDisk(s_Data.distributionSampler0, 32);
        GeneratePoissonDisk(s_Data.distributionSampler1, 32);

        s_Data.lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 500.0f);
    }

    void SceneRenderer::BeginScene(const PerspectiveCamera& camera)
    {   
        s_Data.cameraBuffer.viewProjection = camera.GetViewProjection();
        s_Data.cameraBuffer.position = camera.GetPosition();
        s_Data.cameraUniformBuffer->SetData(&s_Data.cameraBuffer, sizeof(CameraData), 0);

        s_Data.directionalLightBuffer.direction = glm::vec4(0.0f);
        s_Data.directionalLightBuffer.color     = glm::vec4(0.0f);

        for (auto& pointLight : s_Data.pointLightsBuffer)
        {
            pointLight.position = glm::vec4(0.0f);
            pointLight.color    = glm::vec4(0.0f);
        }

        for (auto& spotLight : s_Data.spotLightsBuffer)
        {
            spotLight.position  = glm::vec4(0.0f);
            spotLight.direction = glm::vec4(0.0f);
            spotLight.color     = glm::vec4(0.0f);
        }

        for (auto& lightParams : s_Data.lightsParamsBuffer)
        {
            lightParams.pointLinear    = 0.09f;
            lightParams.pointQuadratic = 0.032f;
            lightParams.spotLinear     = 0.09f;
            lightParams.spotQuadratic  = 0.032f;
            lightParams.innerCutOff    = glm::cos(glm::radians(12.5f));
            lightParams.outerCutOff    = glm::cos(glm::radians(15.0f));
        }


        Renderer::BeginScene();
        s_Data.mainPass->GetSpecification().TargetFrameBuffer->Bind();
        RenderCommand::Clear();
    }

    void SceneRenderer::UpdateLights(Scene& scene)
    {
        auto& lightView = scene.m_Registry.view<TransformComponent, LightComponent>();

        //point light index
        int pIndex = 0;
        //spot light index
        int sIndex = 0;

        for (auto entity : lightView)
        {
            auto& transformComponent = lightView.get<TransformComponent>(entity);
            auto& lightComponent = lightView.get<LightComponent>(entity);

            if (lightComponent.type == LightType::Directional)
            {
                auto light = dynamic_cast<DirectionalLight*>(lightComponent.light.get());
                s_Data.directionalLightBuffer.color = glm::vec4(light->GetColor(), 0.0f) * light->GetIntensity();
                s_Data.directionalLightBuffer.direction = glm::vec4(light->GetDirection(), 0.0f);

                // shadow
                s_Data.lightView = glm::lookAt(glm::vec3(s_Data.directionalLightBuffer.direction), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                s_Data.shadowBuffer.lightViewProjection = s_Data.lightProjection * s_Data.lightView;
                s_Data.shadowUniformBuffer->SetData(&s_Data.shadowBuffer, sizeof(glm::mat4));
                light = nullptr;
            }
            if (lightComponent.type == LightType::Point)
            {
                if (pIndex < 5)
                {
                    auto light = dynamic_cast<PointLight*>(lightComponent.light.get());
                    s_Data.pointLightsBuffer[pIndex].position = glm::vec4(transformComponent.Translation, 1.0f);
                    s_Data.pointLightsBuffer[pIndex].color = glm::vec4(light->GetColor(), 1.0f) * light->GetIntensity();

                    s_Data.lightsParamsBuffer[pIndex].pointLinear = light->GetLinear();
                    s_Data.lightsParamsBuffer[pIndex].pointQuadratic = light->GetQuadratic();

                    pIndex++;
                    light = nullptr;
                }
            }
            if (lightComponent.type == LightType::Spot)
            {
                if (sIndex < 5)
                {
                    auto light = dynamic_cast<SpotLight*>(lightComponent.light.get());
                    s_Data.spotLightsBuffer[sIndex].position = glm::vec4(transformComponent.Translation, 1.0f);
                    s_Data.spotLightsBuffer[sIndex].direction = glm::vec4(transformComponent.Rotation, 0.0f);
                    s_Data.spotLightsBuffer[sIndex].color = glm::vec4(light->GetColor(), 1.0f) * light->GetIntensity();

                    s_Data.lightsParamsBuffer[sIndex].spotLinear = light->GetLinear();
                    s_Data.lightsParamsBuffer[sIndex].spotQuadratic = light->GetQuadratic();
                    s_Data.lightsParamsBuffer[sIndex].innerCutOff = glm::cos(glm::radians(light->GetInnerCutOff()));
                    s_Data.lightsParamsBuffer[sIndex].outerCutOff = glm::cos(glm::radians(light->GetOuterCutOff()));
                
                    sIndex++;
                    light = nullptr;
                }
            }
        }

        s_Data.lightsUniformBuffer->SetData(&s_Data.pointLightsBuffer, sizeof(s_Data.pointLightsBuffer), 0);
        s_Data.lightsUniformBuffer->SetData(&s_Data.spotLightsBuffer, sizeof(s_Data.spotLightsBuffer), sizeof(s_Data.pointLightsBuffer));
        s_Data.lightsUniformBuffer->SetData(&s_Data.directionalLightBuffer, sizeof(s_Data.directionalLightBuffer), sizeof(s_Data.pointLightsBuffer) + sizeof(s_Data.spotLightsBuffer));
        s_Data.lightsParamsUniformBuffer->SetData(&s_Data.lightsParamsBuffer, sizeof(s_Data.lightsParamsBuffer), 0);
    }

    void SceneRenderer::RenderScene(Scene& scene)
    {
        UpdateLights(scene);

        auto view = scene.m_Registry.view<TransformComponent, MeshComponent>();

        // 渲染深度贴图
        s_Data.shadowPass->GetSpecification().TargetFrameBuffer->Bind();
        RenderCommand::SetState(RenderState::DEPTH_TEST, true);
        glClear(GL_DEPTH_BUFFER_BIT);
        for (auto& entity : view)
        {
            auto& transformComponent = view.get<TransformComponent>(entity);
            auto& meshComponent = view.get<MeshComponent>(entity);

            if (!meshComponent.path.empty())
            {
                s_Data.depthShader->Bind();
                s_Data.depthShader->SetMat4("transform.u_Transform", transformComponent.GetTransform());
                SceneRenderer::RenderEntityColor(entity, transformComponent, meshComponent, s_Data.depthShader);
            }
        }
        s_Data.shadowPass->GetSpecification().TargetFrameBuffer->Unbind();

        s_Data.mainPass->GetSpecification().TargetFrameBuffer->Bind();
        RenderCommand::SetState(RenderState::DEPTH_TEST, true);
        RenderCommand::Clear();

        s_Data.basicShader->Bind();
        Texture2D::BindTexture(s_Data.shadowPass->GetSpecification().TargetFrameBuffer->GetDepthAttachmentRendererID(), 3);
        Texture1D::BindTexture(s_Data.distributionSampler0->GetRendererID(), 4);
        Texture1D::BindTexture(s_Data.distributionSampler1->GetRendererID(), 5);
        s_Data.basicShader->SetFloat("push.size", s_Data.lightSize);
        s_Data.basicShader->SetInt("push.numPCFSamples", s_Data.numPCF);
        s_Data.basicShader->SetInt("push.numBlockerSearchSamples", s_Data.numBlocker);
        s_Data.basicShader->SetInt("push.softShadow", (int)s_Data.softShadow);
        for (auto entity : view)
        {
            auto& transformComponent = view.get<TransformComponent>(entity);
            auto& meshComponent = view.get<MeshComponent>(entity);

            if (!meshComponent.path.empty()) // 如果有模型路径
            {
                s_Data.transformBuffer.transform = transformComponent.GetTransform();
                s_Data.transformUniformBuffer->SetData(&s_Data.transformBuffer, sizeof(TransformData), 0);

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
        s_Data.mainPass->GetSpecification().TargetFrameBuffer->Unbind();

        s_Data.mousePass->GetSpecification().TargetFrameBuffer->Bind();
        RenderCommand::Clear();
        s_Data.mousePass->GetSpecification().TargetFrameBuffer->ClearAttachment(0, -1);
        for (auto entity : view)
        {
            auto& transformComponent = view.get<TransformComponent>(entity);
            auto& meshComponent = view.get<MeshComponent>(entity);

            if (!meshComponent.path.empty())
            {
                s_Data.transformBuffer.transform = transformComponent.GetTransform();
                s_Data.transformBuffer.id = (uint32_t)entity;
                s_Data.transformUniformBuffer->SetData(&s_Data.transformBuffer, sizeof(TransformData), 0);

                RenderEntityID(entity, transformComponent, meshComponent);
            }
        }
        s_Data.mousePass->GetSpecification().TargetFrameBuffer->Unbind();
    }

    void SceneRenderer::RenderEntityColor(const entt::entity& entity, TransformComponent& transform, MeshComponent& mesh)
    {
        RenderCommand::SetState(RenderState::CULL, false);
        s_Data.basicShader->Bind(); // glUseProgram
        Renderer::Submit(mesh.model, s_Data.basicShader);
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
        s_Data.mouseShader->Bind();
        Renderer::Submit(mesh.model, s_Data.mouseShader);
    }

    void SceneRenderer::EndScene()
    {
        s_Data.postPass->GetSpecification().TargetFrameBuffer->Bind();
        RenderCommand::Clear();
        RenderCommand::DisableDepthTest();

        s_Data.quadShader->Bind();
        s_Data.quadShader->SetFloat("pc.exposure", s_Data.exposure);
        s_Data.quadShader->SetFloat("pc.gamma", s_Data.gamma);
        Texture2D::BindTexture(s_Data.mainPass->GetSpecification().TargetFrameBuffer->GetColorAttachmentRendererID(), 0);
        Renderer::Submit(s_Data.vertexArray, s_Data.quadShader);
        s_Data.postPass->GetSpecification().TargetFrameBuffer->Unbind();
        Renderer::EndScene();
    }

    void SceneRenderer::OnImGuiUpdate()
    {
        /// ====================== Scene Setting ========================
        ImGui::Begin(u8"场景设置");

        ImGui::DragFloat(u8"曝   光", &s_Data.exposure, 0.001f, -2, 4);

        ImGui::DragFloat(u8"gamma值", &s_Data.gamma, 0.01f, 0, 4);

        ImGui::DragFloat(u8"光源大小", &s_Data.lightSize, 0.0001, 0, 100);

        static bool vSync = true;
        ImGui::Checkbox(u8"垂直同步", &vSync);
        Application::Get().GetWindow().SetVSync(vSync);

        ImGui::Checkbox(u8"软阴影", &s_Data.softShadow);

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
            ImGui::Image(reinterpret_cast<void*>(s_Data.shadowPass->GetSpecification()
                            .TargetFrameBuffer->GetDepthAttachmentRendererID()), viewportPanelSize);
            ImGui::End();
        }
    }

    void SceneRenderer::OnViewportResize(uint32_t width, uint32_t height)
    {
        s_Data.mainPass->GetSpecification().TargetFrameBuffer->Resize(width, height);
        s_Data.mousePass->GetSpecification().TargetFrameBuffer->Resize(width, height);
        s_Data.postPass->GetSpecification().TargetFrameBuffer->Resize(width, height);
    }

    uint32_t SceneRenderer::GetTextureID(int index)
    { 
        return s_Data.postPass->GetSpecification().TargetFrameBuffer->GetColorAttachmentRendererID(index); 
    }

    FramebufferSpecification SceneRenderer::GetMainFBSpec()
    {
        return s_Data.mainPass->GetSpecification().TargetFrameBuffer->GetSpecification();
    }

    std::shared_ptr<FrameBuffer> SceneRenderer::GetMouseFB()
    {
        return s_Data.mousePass->GetSpecification().TargetFrameBuffer;
    }

    ShaderLibrary& SceneRenderer::GetShaderLibrary()
    {
        return s_Data.shaders;
    }
}