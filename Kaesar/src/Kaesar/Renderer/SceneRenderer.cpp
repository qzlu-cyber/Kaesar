#include "krpch.h"
#include "SceneRenderer.h"

#include "Kaesar/Scene/Scene.h"
#include "Kaesar/Renderer/RenderCommand.h"
#include "Kaesar/Renderer/Renderer.h"
#include "Kaesar/Core/Application.h"
#include "Kaesar/Utils/PoissonGenerator.h"
#include "Kaesar/Utils/PlatformUtils.h"
#include "Kaesar/Utils/TransString.h"
#include "Kaesar/ImGui/IconsFontAwesome5.h"

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
        shadowSpec.Width = 4096;
        shadowSpec.Height = 4096;
        shadowSpec.Samples = 1;
        shadowSpec.ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
        
        RenderPassSpecification shadowPassSpec;
        shadowPassSpec.TargetFrameBuffer = FrameBuffer::Create(shadowSpec);
        s_Data.shadowPass = RenderPass::Create(shadowPassSpec);

        ///----------------------------------------------Anti Aliasing-----------------------------------------///
        FramebufferSpecification aaFB;
        aaFB.Attachments = { FramebufferTextureFormat::RGBA8 };
        aaFB.Width = 1920;
        aaFB.Height = 1080;
        aaFB.Samples = 1;
        aaFB.ClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        RenderPassSpecification aaPassSpec;
        aaPassSpec.TargetFrameBuffer = FrameBuffer::Create(aaFB);
        s_Data.aaPass = RenderPass::Create(aaPassSpec);

        ///-------------------------------------------------Quad-----------------------------------------------///
        float quad[] = {
             // positions             // texCoords
             1.0f,  1.0f, 0.0f,    1.0f, 1.0f,   // top right
             1.0f, -1.0f, 0.0f,    1.0f, 0.0f,   // bottom right
            -1.0f, -1.0f, 0.0f,    0.0f, 0.0f,   // bottom left
            -1.0f,  1.0f, 0.0f,    0.0f, 1.0f    // top left 
        };

        unsigned int quadIndices[] = {
            0, 3, 1, // first triangle
            1, 3, 2  // second triangle
        };

        BufferLayout quadLayout = {
            { ShaderDataType::Float3,"a_Position" },
            { ShaderDataType::Float2,"a_TexCoords" },
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
            s_Data.shaders.Load("assets/shaders/FXAA.glsl");
        }
        s_Data.basicShader = s_Data.shaders.Get("basic");
        s_Data.lightShader = s_Data.shaders.Get("light");
        s_Data.geoShader = s_Data.shaders.Get("GeometryPass");
        s_Data.deferredLightingShader = s_Data.shaders.Get("DeferredLighting");
        s_Data.fxaaShader = s_Data.shaders.Get("FXAA");

        s_Data.depthShader = Shader::Create("assets/shaders/depth.glsl");

        ///-------------------------------------------------Uniforms-------------------------------------------///
        s_Data.cameraUniformBuffer = UniformBuffer::Create(sizeof(CameraData), 0); // 将和相机有关的数据绑定在 0 号绑定点
        s_Data.transformUniformBuffer = UniformBuffer::Create(sizeof(TransformData), 1); // 将和变换有关的数据绑定在 1 号绑定点
        s_Data.lightManager = std::make_shared<LightManager>(2);
        s_Data.shadowUniformBuffer = UniformBuffer::Create(sizeof(glm::mat4), 4);

        s_Data.exposure = 0.5f;
        s_Data.gamma = 1.9f;
        s_Data.lightSize = 1.0f;
        s_Data.orthoSize = 20.0f;
        s_Data.lightNear = 20.0f;
        s_Data.lightFar = 200.0f;

        s_Data.intensity = 1.0f;
        
        GeneratePoissonDisk(s_Data.distributionSampler0, 64);
        GeneratePoissonDisk(s_Data.distributionSampler1, 64);
        s_Data.basicShader->Bind();
        Texture1D::BindTexture(s_Data.distributionSampler0->GetRendererID(), 4);
        Texture1D::BindTexture(s_Data.distributionSampler1->GetRendererID(), 5);
        s_Data.basicShader->Unbind();

        float dSize = s_Data.orthoSize;
        //s_Data.lightProjection = glm::perspective(45.0f, 1.0f, s_Data.lightNear, s_Data.lightFar);
        // 透视投影会对深度值产生非线性影响，使用正交投影，可以保证在裁剪过程中保留 z 值的线性性质
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

        s_Data.lightManager->IntitializeLights();

        Renderer::BeginScene();
    }

    void SceneRenderer::UpdateLights()
    {
        auto& lightView = s_Data.scene->m_Registry.view<TransformComponent, LightComponent>();

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
                
                s_Data.lightManager->UpdateDirLight(light, transformComponent.Translation);

                // shadow
                s_Data.lightView = glm::lookAt(-(glm::normalize(light->GetDirection()) * s_Data.lightFar / 4.0f), 
                                               glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                s_Data.shadowBuffer.lightViewProjection = s_Data.lightProjection * s_Data.lightView;
                s_Data.shadowUniformBuffer->SetData(&s_Data.shadowBuffer, sizeof(glm::mat4));
                light = nullptr;
            }
            if (lightComponent.type == LightType::Point)
            {
                if (pIndex < 5)
                {
                    auto light = dynamic_cast<PointLight*>(lightComponent.light.get());
                    
                    s_Data.lightManager->UpdatePointLights(light, transformComponent.Translation, pIndex);

                    pIndex++;
                    light = nullptr;
                }
            }
            if (lightComponent.type == LightType::Spot)
            {
                if (sIndex < 5)
                {
                    auto light = dynamic_cast<SpotLight*>(lightComponent.light.get());
                    
                    s_Data.lightManager->UpdateSpotLights(light, transformComponent.Translation, sIndex);
                
                    sIndex++;
                    light = nullptr;
                }
            }
        }

        s_Data.lightManager->UpdateBuffer();
    }

    void SceneRenderer::RenderScene()
    {
        UpdateLights();

        auto view = s_Data.scene->m_Registry.view<TransformComponent, MeshComponent>();

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
                SceneRenderer::RenderEntity(entity, meshComponent, s_Data.depthShader);
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

                if (s_Data.scene->m_Registry.has<MaterialComponent>(entity)) // 如果有材质组件
                {
                    auto& materialComponent = s_Data.scene->m_Registry.get<MaterialComponent>(entity);
                    s_Data.geoShader->SetInt("transform.id", (uint32_t)entity);
                    s_Data.geoShader->SetMat4("transform.u_Transform", transformComponent.GetTransform());
                    SceneRenderer::RenderEntity(entity, meshComponent, materialComponent); // 使用它自身的材质组件渲染
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
                    SceneRenderer::RenderEntity(entity, meshComponent, s_Data.geoShader); // 否则使用默认渲染
                }
            }
        }
        s_Data.geoShader->Unbind();
        s_Data.geoPass->UnbindTargetFrameBuffer();
    }

    void SceneRenderer::SetScene(const std::shared_ptr<Scene>& scene)
    {
        s_Data.scene = scene;

        auto& path = scene->m_EnvironmentPath;
        if (!path.empty())
        {
            s_Data.environment = std::make_shared<Environment>(Texture2D::CreateHDR(path, 1, false, true));
        }
        else
        {
            s_Data.environment = nullptr;
        }
    }

    void SceneRenderer::RenderEntity(const entt::entity& entity, MeshComponent& mesh, MaterialComponent& material)
    {
        Renderer::Submit(material.material, mesh.model);
    }

    void SceneRenderer::RenderEntity(const entt::entity& entity, MeshComponent& mc, const std::shared_ptr<Shader>& shader)
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
        s_Data.deferredLightingShader->SetFloat("pc.size", s_Data.lightSize);
        s_Data.deferredLightingShader->SetInt("pc.numPCFSamples", s_Data.numPCF);
        s_Data.deferredLightingShader->SetInt("pc.numBlockerSearchSamples", s_Data.numBlocker);
        s_Data.deferredLightingShader->SetInt("pc.softShadow", (int)s_Data.softShadow);
        s_Data.deferredLightingShader->SetFloat("pc.exposure", s_Data.exposure);
        s_Data.deferredLightingShader->SetFloat("pc.gamma", s_Data.gamma);
        s_Data.deferredLightingShader->SetFloat("pc.near", s_Data.lightNear);
        s_Data.deferredLightingShader->SetFloat("pc.intensity", s_Data.intensity);

        //GBuffer samplers
        Texture2D::BindTexture(s_Data.geoPass->GetFrameBufferTextureID(0), 0);
        Texture2D::BindTexture(s_Data.geoPass->GetFrameBufferTextureID(1), 1);
        Texture2D::BindTexture(s_Data.geoPass->GetFrameBufferTextureID(2), 2);
        Texture2D::BindTexture(s_Data.geoPass->GetFrameBufferTextureID(3), 6);

        if (s_Data.environment)
        {
            s_Data.environment->BindIrradianceMap(7);
            s_Data.environment->BindPreFilterMap(8);
            s_Data.environment->BindBRDFMap(9);
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

        s_Data.aaPass->BindTargetFrameBuffer();
        s_Data.fxaaShader->Bind();
        s_Data.fxaaShader->SetFloat("pc.width", (float)s_Data.aaPass->GetSpecification().TargetFrameBuffer->GetSpecification().Width);
        s_Data.fxaaShader->SetFloat("pc.height", (float)s_Data.aaPass->GetSpecification().TargetFrameBuffer->GetSpecification().Height);
        Texture2D::BindTexture(s_Data.lightingPass->GetFrameBufferTextureID(0), 0);
        Renderer::Submit(s_Data.vertexArray, s_Data.fxaaShader);
        s_Data.fxaaShader->Unbind();
        s_Data.aaPass->UnbindTargetFrameBuffer();

        Renderer::EndScene();
    }

    void SceneRenderer::OnImGuiUpdate()
    {
        /// ====================== Scene Setting ========================
        std::stringstream ss;
        std::string title = u8" 场景设置";
        title = TransString::TBS(title);
        ss << ICON_FA_DHARMACHAKRA << title;
        ImGui::Begin(ss.str().c_str());

        std::string label = "shader";
        static std::shared_ptr<Shader> selectedShader;
        if (selectedShader)
        {
            label = selectedShader->GetName();
        }
        static int item_current_idx = 0;
        static int index = 0;
        if (ImGui::BeginCombo("##Shaders", label.c_str()))
        {
            for (auto& shader : s_Data.shaders.GetShaders())
            {
                //const bool is_selected = (item_current_idx == n);
                if (ImGui::Selectable(shader.first.c_str(), true)) {
                    selectedShader = shader.second;
                }

                ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        if (ImGui::Button("Reload shader")) {
            Reload(selectedShader);
        }
        ImGui::Separator();


        ImGui::DragFloat(u8"曝   光", &s_Data.exposure, 0.001f, -2, 4);

        ImGui::DragFloat(u8"gamma值", &s_Data.gamma, 0.01f, 0, 4);

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
                s_Data.scene->m_EnvironmentPath = *path;
            }
        }

        if (ImGui::DragFloat("Intensity", &s_Data.intensity, 0.01f, 1, 20))
        {
            if (s_Data.environment)
            {
                s_Data.environment->SetIntensity(s_Data.intensity);
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

        ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
        if (ImGui::DragFloat("near", &s_Data.lightNear, 0.01f, 0.1f, 100.0f))
        {
            s_Data.lightProjection = glm::perspective(45.0f, 1.0f, s_Data.lightNear, s_Data.lightFar);
        }
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::DragFloat("far", &s_Data.lightFar, 0.1f, 100.0f, 10000.0f))
        {          
            s_Data.lightProjection = glm::perspective(45.0f, 1.0f, s_Data.lightNear, s_Data.lightFar);
        }
        ImGui::PopItemWidth();

        ImGui::End();
    }

    void SceneRenderer::OnViewportResize(uint32_t width, uint32_t height)
    {
        s_Data.geoPass->GetSpecification().TargetFrameBuffer->Resize(width, height);
        s_Data.lightingPass->GetSpecification().TargetFrameBuffer->Resize(width, height);
        s_Data.aaPass->GetSpecification().TargetFrameBuffer->Resize(width, height);
    }

    uint32_t SceneRenderer::GetTextureID(int index)
    { 
        return s_Data.aaPass->GetSpecification().TargetFrameBuffer->GetColorAttachmentRendererID(index); 
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

    void SceneRenderer::Reload(const std::shared_ptr<Shader>& shader)
    {
        shader->Reload();
    }
}