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
        PoissonGenerator::DefaultPRNG PRNG; // ���� Poisson-disc �ֲ��������������
        size_t attempts = 0; // ���� Poisson-disc �ֲ��ĳ��Դ���
        // ʹ�� Poisson Disk Sampling �������� numSamples * 2 ���㣬������������� points ��
        auto points = PoissonGenerator::generatePoissonPoints(numSamples * 2, PRNG);

        // ������ɵĵ��������� numSamples �������ٴ�����
        while (points.size() < numSamples && ++attempts < 100)
            auto points = PoissonGenerator::generatePoissonPoints(numSamples * 2, PRNG);

        // ������Դ����ﵽ 100 ����Ȼ�޷������㹻�ĵ㣬�򱨴�
        if (attempts == 100)
        {
            KR_CORE_ERROR("�޷����� {0} �� Poisson-disc �ֲ������㣡", numSamples);
            numSamples = points.size();
        }

        std::vector<float> data(numSamples * 2); // ���ڱ��������������
        for (auto i = 0, j = 0; i < numSamples; i++, j += 2)
        {
            auto& point = points[i];
            // ��������� x, y ���걣���� data ��
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
        // ������Ӱ֡����
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
        s_Data.cameraUniformBuffer = UniformBuffer::Create(sizeof(CameraData), 0); // ��������йص����ݰ��� 0 �Ű󶨵�
        s_Data.transformUniformBuffer = UniformBuffer::Create(sizeof(TransformData), 1); // ���ͱ任�йص����ݰ��� 1 �Ű󶨵�
        s_Data.lightsUniformBuffer = UniformBuffer::Create(sizeof(s_Data.pointLightBuffer) + sizeof(s_Data.spotLightBuffer) + sizeof(s_Data.directionalLightBuffer), 2);
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

        s_Data.directionalLightBuffer.direction = glm::vec3(0.0f, 0.0f, 0.0f);
        s_Data.directionalLightBuffer.ambient   = glm::vec3(0.0f, 0.0f, 0.0f);
        s_Data.directionalLightBuffer.diffuse   = glm::vec3(0.0f, 0.0f, 0.0f);
        s_Data.directionalLightBuffer.specular  = glm::vec3(0.0f, 0.0f, 0.0f);

        s_Data.pointLightBuffer.position = glm::vec3(0.0f, 0.0f, 0.0f);
        s_Data.pointLightBuffer.ambient  = glm::vec3(0.0f, 0.0f, 0.0f);
        s_Data.pointLightBuffer.diffuse  = glm::vec3(0.0f, 0.0f, 0.0f);
        s_Data.pointLightBuffer.specular = glm::vec3(0.0f, 0.0f, 0.0f);

        s_Data.spotLightBuffer.position  = glm::vec3(0.0f, 0.0f, 0.0f);
        s_Data.spotLightBuffer.direction = glm::vec3(0.0f, 0.0f, 0.0f);
        s_Data.spotLightBuffer.ambient   = glm::vec3(0.0f, 0.0f, 0.0f);
        s_Data.spotLightBuffer.diffuse   = glm::vec3(0.0f, 0.0f, 0.0f);
        s_Data.spotLightBuffer.specular  = glm::vec3(0.0f, 0.0f, 0.0f);

        s_Data.lightsParamsBuffer.dirIntensity   = 0.0f;
        s_Data.lightsParamsBuffer.pointLinear    = 0.0f;
        s_Data.lightsParamsBuffer.pointQuadratic = 0.0f;
        s_Data.lightsParamsBuffer.spotLinear     = 0.0f;
        s_Data.lightsParamsBuffer.spotQuadratic  = 0.0f;
        s_Data.lightsParamsBuffer.innerCutOff    = 0.0f;
        s_Data.lightsParamsBuffer.outerCutOff    = 0.0f;

        Renderer::BeginScene();
        s_Data.mainPass->GetSpecification().TargetFrameBuffer->Bind();
        RenderCommand::Clear();
    }

    void SceneRenderer::UpdateLights(Scene& scene)
    {
        auto& lightView = scene.m_Registry.view<TransformComponent, LightComponent>();

        for (auto entity : lightView)
        {
            auto& transformComponent = lightView.get<TransformComponent>(entity);
            auto& lightComponent = lightView.get<LightComponent>(entity);

            if (lightComponent.type == LightType::Directional)
            {
                auto light = dynamic_cast<DirectionalLight*>(lightComponent.light.get());
                s_Data.directionalLightBuffer.ambient = light->GetAmbient() * light->GetIntensity();
                s_Data.directionalLightBuffer.diffuse = light->GetDiffuse() * light->GetIntensity();
                s_Data.directionalLightBuffer.specular = light->GetSpecular() * light->GetIntensity();
                s_Data.directionalLightBuffer.direction = light->GetDirection();

                // shadow
                s_Data.lightView = glm::lookAt(glm::vec3(s_Data.directionalLightBuffer.direction), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                s_Data.shadowBuffer.lightViewProjection = s_Data.lightProjection * s_Data.lightView;
                s_Data.shadowUniformBuffer->SetData(&s_Data.shadowBuffer, sizeof(glm::mat4));

                s_Data.lightsParamsBuffer.dirIntensity = light->GetIntensity();
            }
            if (lightComponent.type == LightType::Point)
            {
                auto light = dynamic_cast<PointLight*>(lightComponent.light.get());
                s_Data.pointLightBuffer.position = transformComponent.Translation;

                s_Data.pointLightBuffer.ambient = light->GetAmbient() * light->GetIntensity();
                s_Data.pointLightBuffer.diffuse = light->GetDiffuse() * light->GetIntensity();
                s_Data.pointLightBuffer.specular = light->GetSpecular() * light->GetIntensity();

                s_Data.lightsParamsBuffer.pointLinear = light->GetLinear();
                s_Data.lightsParamsBuffer.pointQuadratic = light->GetQuadratic();
            }
            if (lightComponent.type == LightType::Spot)
            {
                auto light = dynamic_cast<SpotLight*>(lightComponent.light.get());
                s_Data.spotLightBuffer.position = transformComponent.Translation;
                s_Data.spotLightBuffer.direction = transformComponent.Rotation;
                s_Data.spotLightBuffer.ambient = light->GetAmbient() * light->GetIntensity();
                s_Data.spotLightBuffer.diffuse = light->GetDiffuse() * light->GetIntensity();
                s_Data.spotLightBuffer.specular = light->GetSpecular() * light->GetIntensity();

                s_Data.lightsParamsBuffer.spotLinear = light->GetLinear();
                s_Data.lightsParamsBuffer.spotQuadratic = light->GetQuadratic();
                s_Data.lightsParamsBuffer.innerCutOff = glm::cos(glm::radians(light->GetInnerCutOff()));
                s_Data.lightsParamsBuffer.outerCutOff = glm::cos(glm::radians(light->GetOuterCutOff()));
            }
        }

        s_Data.lightsUniformBuffer->SetData(&s_Data.pointLightBuffer, sizeof(s_Data.pointLightBuffer), 0);
        s_Data.lightsUniformBuffer->SetData(&s_Data.spotLightBuffer, sizeof(s_Data.spotLightBuffer), sizeof(s_Data.pointLightBuffer));
        s_Data.lightsUniformBuffer->SetData(&s_Data.directionalLightBuffer, sizeof(s_Data.directionalLightBuffer), sizeof(s_Data.pointLightBuffer) + sizeof(s_Data.spotLightBuffer));
        s_Data.lightsParamsUniformBuffer->SetData(&s_Data.lightsParamsBuffer, sizeof(s_Data.lightsParamsBuffer), 0);
    }

    void SceneRenderer::RenderScene(Scene& scene)
    {
        UpdateLights(scene);

        auto view = scene.m_Registry.view<TransformComponent, MeshComponent>();

        // ��Ⱦ�����ͼ
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

            if (!meshComponent.path.empty()) // �����ģ��·��
            {
                s_Data.transformBuffer.transform = transformComponent.GetTransform();
                s_Data.transformUniformBuffer->SetData(&s_Data.transformBuffer, sizeof(TransformData), 0);

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
        ImGui::Begin(u8"��������");

        ImGui::DragFloat(u8"��   ��", &s_Data.exposure, 0.001f, -2, 4);

        ImGui::DragFloat(u8"gammaֵ", &s_Data.gamma, 0.01f, 0, 4);

        ImGui::DragFloat(u8"��Դ��С", &s_Data.lightSize, 0.0001, 0, 100);

        static bool vSync = true;
        ImGui::Checkbox(u8"��ֱͬ��", &vSync);
        Application::Get().GetWindow().SetVSync(vSync);

        ImGui::Checkbox(u8"����Ӱ", &s_Data.softShadow);

        //DepthMap
        static bool showDepth = false;
        if (ImGui::Button(u8"�����ͼ"))
        {
            showDepth = !showDepth;
        }

        ImGui::End();

        if (showDepth) 
        {
            ImGui::Begin(u8"�����ͼ");
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