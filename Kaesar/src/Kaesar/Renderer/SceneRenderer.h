#pragma once

#include "Kaesar/Renderer/Camera/PerspectiveCamera.h"
#include "Kaesar/Renderer/VertexArray.h"
#include "Kaesar/Renderer/Buffer.h"
#include "Kaesar/Renderer/Shader.h"
#include "Kaesar/Renderer/FrameBuffer.h"
#include "Kaesar/Renderer/UniformBuffer.h"
#include "Kaesar/Scene/Component.h"
#include "Kaesar/Renderer/RenderPass.h"
#include "Kaesar/Renderer/Environment.h"
#include "Kaesar/Renderer/LightManager.h"

#include <entt.hpp>
#include <memory>
#include <glm/glm.hpp>

namespace Kaesar {
    class Entity;
    class Scene;

    class SceneRenderer
    {
    public:
        static void Initialize();
        static void UpdateLights();
        static void BeginScene(const PerspectiveCamera& camera);
        static void RenderScene();
        static void SetScene(const std::shared_ptr<Scene>& scene);
        static void RenderEntity(const entt::entity& entity, MeshComponent& mesh, MaterialComponent& material);
        static void RenderEntity(const entt::entity& entity, MeshComponent& mesh, const std::shared_ptr<Shader>& shader);
        static void EndScene();

        static void OnImGuiUpdate();

        static void OnViewportResize(uint32_t width, uint32_t height);

        static uint32_t GetTextureID(int index);

        static FramebufferSpecification GetMainFrameSpec();
        static std::shared_ptr<FrameBuffer> GetGeoFrameBuffer();

        static ShaderLibrary& GetShaderLibrary();

        static void Reload(const std::shared_ptr<Shader>& shader);

    public:
        struct CameraData
        {
            glm::mat4 viewProjection;
            glm::vec3 position;
        };

        struct TransformData
        {
            glm::mat4 transform;
        };

        struct ShadowData
        {
            glm::mat4 lightViewProjection;
        };

        struct SceneData
        {
            // scene
            std::shared_ptr<Scene> scene;

            CameraData cameraBuffer;
            TransformData transformBuffer;

            // environment
            float intensity;
            std::shared_ptr<Environment> environment;

            // lights
            float exposure; // 曝光度
            float gamma; // gamma 矫正
            float lightSize;
            std::shared_ptr<LightManager> lightManager;

            // shadow
            bool softShadow = true;
            float numPCF = 16;
            float numBlocker = 2;
            float orthoSize;
            float lightNear;
            float lightFar;
            glm::mat4 lightProjection;
            glm::mat4 lightView;
            ShadowData shadowBuffer;

            std::shared_ptr<VertexArray> vertexArray;

            // Poisson-disk Distribution
            std::shared_ptr<Texture1D> distributionSampler0, distributionSampler1; // 用于 PCF 阴影采样

            // shader
            ShaderLibrary shaders;
            std::shared_ptr<Shader> basicShader, lightShader, depthShader, geoShader, 
                                    deferredLightingShader, fxaaShader;

            // FrameBuffers
            int textureRenderSlot = 2;
            std::shared_ptr<RenderPass> geoPass, shadowPass, lightingPass, aaPass;

            std::shared_ptr<UniformBuffer> cameraUniformBuffer, transformUniformBuffer, shadowUniformBuffer;

            glm::vec3 clearColor;
        };

        friend class ScenePanel;
        friend class SceneSerializer;
    };
}