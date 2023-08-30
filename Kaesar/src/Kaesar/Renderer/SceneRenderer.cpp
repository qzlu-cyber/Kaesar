#include "krpch.h"
#include "SceneRenderer.h"

#include "Kaesar/Scene/Scene.h"
#include "Kaesar/Renderer/RenderCommand.h"
#include "Kaesar/Renderer/Renderer.h"
#include "Kaesar/Core/Application.h"
#include "Kaesar/Utils/PoissonGenerator.h"
#include "Kaesar/Utils/PlatformUtils.h"

#include <glad/glad.h>
#include "imgui.h"
#include "imgui_internal.h"

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
        ///------------------------------------------Deferred Geometry Render Pass----------------------------------///
        FramebufferSpecification GeoFbSpec;
        GeoFbSpec.Attachments =
        {
            FramebufferTextureFormat::RGBA16F,			// 坐标颜色缓冲
            FramebufferTextureFormat::RGBA16F,			// 法线颜色缓冲
            FramebufferTextureFormat::RGBA16F,			// 颜色 + 高光颜色缓冲
            FramebufferTextureFormat::RGBA16F,          // 粗糙度 + 金属度 + AO 缓冲
            FramebufferTextureFormat::RED_INTEGER,      // 物体 ID 缓冲
            FramebufferTextureFormat::DEPTH24STENCIL8	// 深度 + 模板缓冲
        };
        GeoFbSpec.Width = 1920;
        GeoFbSpec.Height = 1080;
        GeoFbSpec.Samples = 1;
        GeoFbSpec.ClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        RenderPassSpecification GeoPassSpec;
        GeoPassSpec.TargetFrameBuffer = FrameBuffer::Create(GeoFbSpec);
        s_Data.geoPass = RenderPass::Create(GeoPassSpec);

        ///--------------------------------------Lighting and Post Processing Pass--------------------------------///
        FramebufferSpecification postProcFB;
        postProcFB.Attachments = { FramebufferTextureFormat::RGBA8 , FramebufferTextureFormat::DEPTH24STENCIL8 };
        postProcFB.Width = 1920;
        postProcFB.Height = 1080;
        postProcFB.Samples = 1;
        postProcFB.ClearColor = glm::vec4(0.196f, 0.196f, 0.196f, 1.0f);

        RenderPassSpecification finalPassSpec;
        finalPassSpec.TargetFrameBuffer = FrameBuffer::Create(postProcFB);
        s_Data.lightingPass = RenderPass::Create(finalPassSpec);

        ///------------------------------------------------Shadow Pass------------------------------------------///
        // 创建阴影帧缓冲
        FramebufferSpecification shadowSpec;
        shadowSpec.Attachments = { FramebufferTextureFormat::DEPTH32 };
        shadowSpec.Width = 1024;
        shadowSpec.Height = 1024;
        shadowSpec.Samples = 1;
        shadowSpec.ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
        
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
            s_Data.shaders.Load("assets/shaders/light.glsl");
            s_Data.shaders.Load("assets/shaders/DeferredLighting.glsl");
            s_Data.shaders.Load("assets/shaders/GeometryPass.glsl");
        }
        s_Data.basicShader = s_Data.shaders.Get("basic");
        s_Data.lightShader = s_Data.shaders.Get("light");
        s_Data.geoShader = s_Data.shaders.Get("GeometryPass");
        s_Data.deferredLightingShader = s_Data.shaders.Get("DeferredLighting");
        s_Data.mouseShader = Shader::Create("assets/shaders/mouse.glsl");
        s_Data.quadShader  = Shader::Create("assets/shaders/quad.glsl");
        s_Data.depthShader = Shader::Create("assets/shaders/depth.glsl");

        ///-------------------------------------------------Uniforms-------------------------------------------///
        s_Data.cameraUniformBuffer = UniformBuffer::Create(sizeof(CameraData), 0); // 将和相机有关的数据绑定在 0 号绑定点
        s_Data.transformUniformBuffer = UniformBuffer::Create(sizeof(TransformData), 1); // 将和变换有关的数据绑定在 1 号绑定点
        s_Data.lightsUniformBuffer = UniformBuffer::Create(sizeof(s_Data.pointLightsBuffer) + sizeof(s_Data.spotLightsBuffer) + sizeof(s_Data.directionalLightBuffer), 2);
        s_Data.lightsParamsUniformBuffer = UniformBuffer::Create(sizeof(s_Data.lightsParamsBuffer), 3);
        s_Data.shadowUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4), 4);

        s_Data.exposure = 0.5f;
        s_Data.gamma = 1.9f;
        s_Data.lightSize = 2.0f;
        s_Data.orthoSize = 10.0f;
        s_Data.lightNear = 1.0f;
        s_Data.lightFar = 500.0f;
        
        GeneratePoissonDisk(s_Data.distributionSampler0, 32);
        GeneratePoissonDisk(s_Data.distributionSampler1, 32);
        s_Data.basicShader->Bind();
        Texture1D::BindTexture(s_Data.distributionSampler0->GetRendererID(), 4);
        Texture1D::BindTexture(s_Data.distributionSampler1->GetRendererID(), 5);
        s_Data.basicShader->Unbind();

        float dSize = s_Data.orthoSize;
        s_Data.lightProjection = glm::ortho(-dSize, dSize, -dSize, dSize, s_Data.lightNear, s_Data.lightFar);
    }

    void SceneRenderer::BeginScene(const PerspectiveCamera& camera)
    {   
        if (s_Data.environment)
        {
            s_Data.environment->SetViewProjection(camera.GetViewMatrix(), camera.GetProjection());
        }

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
                s_Data.lightView = glm::lookAt(-(glm::vec3(s_Data.directionalLightBuffer.direction) * 10.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
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
        s_Data.shadowPass->BindTargetFrameBuffer();
        RenderCommand::SetState(RenderState::DEPTH_TEST, true);
        RenderCommand::SetClearColor(s_Data.shadowPass->GetSpecification().TargetFrameBuffer->GetSpecification().ClearColor);
        s_Data.depthShader->Bind();
        RenderCommand::Clear();
        for (auto& entity : view)
        {
            auto& transformComponent = view.get<TransformComponent>(entity);
            auto& meshComponent = view.get<MeshComponent>(entity);

            if (!meshComponent.path.empty())
            {
                s_Data.depthShader->SetMat4("transform.u_Transform", transformComponent.GetTransform());
                SceneRenderer::RenderEntityColor(entity, transformComponent, meshComponent, s_Data.depthShader);
            }
        }
        s_Data.depthShader->Unbind();
        s_Data.shadowPass->UnbindTargetFrameBuffer();

        s_Data.geoPass->BindTargetFrameBuffer();
        RenderCommand::SetState(RenderState::DEPTH_TEST, true);
        RenderCommand::SetClearColor(s_Data.geoPass->GetSpecification().TargetFrameBuffer->GetSpecification().ClearColor);
        s_Data.geoPass->GetSpecification().TargetFrameBuffer->ClearAttachment(4, -1);
        s_Data.geoShader->Bind();
        RenderCommand::Clear();
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
                    s_Data.geoShader->SetInt("transform.id", (uint32_t)entity);
                    s_Data.geoShader->SetMat4("transform.u_Transform", transformComponent.GetTransform());
                    SceneRenderer::RenderEntityColor(entity, transformComponent, meshComponent, materialComponent); // 使用它自身的材质组件渲染
                }
                else
                {
                    s_Data.geoShader->SetFloat("pc.tiling", 1.0f);
                    s_Data.geoShader->SetInt("pc.HasAlbedoMap", 1);
                    s_Data.geoShader->SetInt("pc.HasNormalMap", 0);
                    s_Data.geoShader->SetInt("pc.HasMetallicMap", 0);
                    s_Data.geoShader->SetInt("pc.HasRoughnessMap", 0);
                    s_Data.geoShader->SetInt("pc.HasAOMap", 0);
                    s_Data.geoShader->SetFloat("pc.material.MetallicFactor", 0.0f);
                    s_Data.geoShader->SetFloat("pc.material.RoughnessFactor", 1.0f);
                    s_Data.geoShader->SetFloat("pc.material.AO", 1.0f);
                    s_Data.geoShader->SetInt("transform.id", (uint32_t)entity);
                    s_Data.geoShader->SetMat4("transform.u_Transform", transformComponent.GetTransform());
                    SceneRenderer::RenderEntityColor(entity, transformComponent, meshComponent, s_Data.geoShader); // 否则使用默认渲染
                }
            }
        }
        s_Data.geoShader->Unbind();
        s_Data.geoPass->UnbindTargetFrameBuffer();
    }

    void SceneRenderer::RenderEntityColor(const entt::entity& entity, TransformComponent& transform, MeshComponent& mesh, MaterialComponent& material)
    {
        Renderer::Submit(material.material, mesh.model);
    }

    void SceneRenderer::RenderEntityColor(const entt::entity& entity, TransformComponent& tc, MeshComponent& mc, const std::shared_ptr<Shader>& shader)
    {
        shader->Bind();
        Renderer::Submit(mc.model, shader);
    }

    void SceneRenderer::EndScene()
    {
        s_Data.lightingPass->BindTargetFrameBuffer();
        RenderCommand::SetState(RenderState::DEPTH_TEST, false);
        RenderCommand::Clear();

        s_Data.deferredLightingShader->Bind();

        //shadow map samplers
        Texture2D::BindTexture(s_Data.shadowPass->GetSpecification().TargetFrameBuffer->GetDepthAttachmentRendererID(), 3);
        Texture1D::BindTexture(s_Data.distributionSampler0->GetRendererID(), 4);
        Texture1D::BindTexture(s_Data.distributionSampler1->GetRendererID(), 5);

        //Push constant variables
        s_Data.deferredLightingShader->SetFloat("pc.size", s_Data.lightSize * 0.0001f);
        s_Data.deferredLightingShader->SetInt("pc.numPCFSamples", s_Data.numPCF);
        s_Data.deferredLightingShader->SetInt("pc.numBlockerSearchSamples", s_Data.numBlocker);
        s_Data.deferredLightingShader->SetInt("pc.softShadow", (int)s_Data.softShadow);
        s_Data.deferredLightingShader->SetFloat("pc.exposure", s_Data.exposure);
        s_Data.deferredLightingShader->SetFloat("pc.gamma", s_Data.gamma);
        s_Data.deferredLightingShader->SetFloat("pc.near", s_Data.lightNear);

        //GBuffer samplers
        Texture2D::BindTexture(s_Data.geoPass->GetFrameBufferTextureID(0), 0);
        Texture2D::BindTexture(s_Data.geoPass->GetFrameBufferTextureID(1), 1);
        Texture2D::BindTexture(s_Data.geoPass->GetFrameBufferTextureID(2), 2);
        Texture2D::BindTexture(s_Data.geoPass->GetFrameBufferTextureID(3), 6);

        if (s_Data.environment)
        {
            s_Data.environment->BindIrradianceMap(7);
        }

        Renderer::Submit(s_Data.vertexArray, s_Data.deferredLightingShader);

        s_Data.deferredLightingShader->Unbind();

        glBindFramebuffer(GL_READ_FRAMEBUFFER, s_Data.geoPass->GetSpecification().TargetFrameBuffer->GetRendererID());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_Data.lightingPass->GetSpecification().TargetFrameBuffer->GetRendererID()); // write to default framebuffer
        auto w = s_Data.lightingPass->GetSpecification().TargetFrameBuffer->GetSpecification().Width;
        auto h = s_Data.lightingPass->GetSpecification().TargetFrameBuffer->GetSpecification().Height;
        glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

        if (s_Data.environment)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL); // 深度缓冲会填上天空和的深度 1.0，所以需要保证天空盒在值小于或等于深度缓冲而不是小于时通过深度测试
            s_Data.environment->RenderBackground();
            glDepthFunc(GL_LESS);
        }

        s_Data.lightingPass->UnbindTargetFrameBuffer();

        Renderer::EndScene();
    }

    void SceneRenderer::OnImGuiUpdate()
    {
        /// ====================== Scene Setting ========================
        ImGui::Begin(u8"场景设置");

        ImGui::DragFloat(u8"曝   光", &s_Data.exposure, 0.001f, -2, 4);

        ImGui::DragFloat(u8"gamma值", &s_Data.gamma, 0.01f, 0, 4);

        ImGui::DragFloat(u8"光源大小", &s_Data.lightSize, 0.0001, 0, 100);

        static bool showDepth = false;
        static bool showAlbedo = false;
        static bool showNormal = false;
        static bool showPosition = false;
        static bool showRoughMetalAO = false;

        if (ImGui::Button(u8"深度贴图"))
        {
            showDepth = !showDepth;
        }
        ImGui::SameLine();
        if (ImGui::Button("Albedo"))
        {
            showAlbedo = !showAlbedo;
        }
        ImGui::SameLine();
        if (ImGui::Button("Normal"))
        {
            showNormal = !showNormal;
        }
        ImGui::SameLine();
        if (ImGui::Button("Position"))
        {
            showPosition = !showPosition;
        }
        ImGui::SameLine();
        if (ImGui::Button("RoughMetalAO"))
        {
            showRoughMetalAO = !showRoughMetalAO;
        }
        auto width = s_Data.geoPass->GetSpecification().TargetFrameBuffer->GetSpecification().Width * 0.5f;
        auto height = s_Data.geoPass->GetSpecification().TargetFrameBuffer->GetSpecification().Height * 0.5f;
        ImVec2 frameSize = ImVec2{ width, height };
        if (showDepth)
        {
            ImGui::Begin(u8"深度贴图");
            ImGui::Image(reinterpret_cast<void*>(s_Data.shadowPass->GetSpecification()
                .TargetFrameBuffer->GetDepthAttachmentRendererID()), frameSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
            ImGui::End();
        }

        if (showPosition)
        {
            ImGui::Begin("Position");
            ImGui::Image(reinterpret_cast<void*>(s_Data.geoPass->GetFrameBufferTextureID(0)), 
                         frameSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
            ImGui::End();
        }
        if (showNormal)
        {
            ImGui::Begin("Normal");
            ImGui::Image(reinterpret_cast<void*>(s_Data.geoPass->GetFrameBufferTextureID(1)), 
                         frameSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
            ImGui::End();
        }
        if (showAlbedo)
        {
            ImGui::Begin("Albedo");
            ImGui::Image(reinterpret_cast<void*>(s_Data.geoPass->GetFrameBufferTextureID(2)), 
                         frameSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
            ImGui::End();
        }

        if (showRoughMetalAO)
        {
            ImGui::Begin("RoughMetalAO");
            ImGui::Image(reinterpret_cast<void*>(s_Data.geoPass->GetFrameBufferTextureID(3)), 
                         frameSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
            ImGui::End();
        }

        if (ImGui::Button("HDR", { 40, 20 }))
        {
            auto path = FileDialogs::OpenFile("HDR (*.hdr)\0*.hdr\0");
            if (path)
            {
                s_Data.environment = std::make_shared<Environment>(Texture2D::CreateHDR(*path, 1, false, true));
            }
        }

        static bool vSync = true;
        ImGui::Checkbox(u8"垂直同步", &vSync);
        Application::Get().GetWindow().SetVSync(vSync);

        //shadow
        ImGui::Checkbox(u8"软阴影", &s_Data.softShadow);
        ImGui::DragFloat(u8"PCF 样本数", &s_Data.numPCF, 1, 1, 64);
        ImGui::DragFloat(u8"Blocker 样本数", &s_Data.numBlocker, 1, 1, 64);
        ImGui::DragFloat(u8"光源大小", &s_Data.lightSize, 0.01f, 0, 100);

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        if (ImGui::DragFloat("near", &s_Data.lightNear, 0.01f, 0.1f, 100.0f))
        {
            s_Data.lightProjection = glm::ortho(-s_Data.orthoSize, s_Data.orthoSize, -s_Data.orthoSize, s_Data.orthoSize, s_Data.lightNear, s_Data.lightFar);
        }
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::DragFloat("far", &s_Data.lightFar, 0.1f, 100.0f, 10000.0f))
        {
            s_Data.lightProjection = glm::ortho(-s_Data.orthoSize, s_Data.orthoSize, -s_Data.orthoSize, s_Data.orthoSize, s_Data.lightNear, s_Data.lightFar);
        }
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::DragFloat("camera size", &s_Data.orthoSize, 0.1f, 1, 50))
        {
            s_Data.lightProjection = glm::ortho(-s_Data.orthoSize, s_Data.orthoSize, -s_Data.orthoSize, s_Data.orthoSize, s_Data.lightNear, s_Data.lightFar);
        }
        ImGui::PopItemWidth();

        ImGui::End();
    }

    void SceneRenderer::OnViewportResize(uint32_t width, uint32_t height)
    {
        s_Data.geoPass->GetSpecification().TargetFrameBuffer->Resize(width, height);
        s_Data.lightingPass->GetSpecification().TargetFrameBuffer->Resize(width, height);
    }

    uint32_t SceneRenderer::GetTextureID(int index)
    { 
        return s_Data.lightingPass->GetSpecification().TargetFrameBuffer->GetColorAttachmentRendererID(index); 
    }

    FramebufferSpecification SceneRenderer::GetMainFrameSpec()
    {
        return s_Data.geoPass->GetSpecification().TargetFrameBuffer->GetSpecification();
    }

    std::shared_ptr<Kaesar::FrameBuffer> SceneRenderer::GetGeoFrameBuffer()
    {
        return s_Data.geoPass->GetSpecification().TargetFrameBuffer;
    }

    ShaderLibrary& SceneRenderer::GetShaderLibrary()
    {
        return s_Data.shaders;
    }
}