#pragma once

#include "Kaesar/Renderer/Camera/PerspectiveCamera.h"
#include "Kaesar/Renderer/VertexArray.h"
#include "Kaesar/Renderer/Buffer.h"
#include "Kaesar/Renderer/Shader.h"
#include "Kaesar/Renderer/FrameBuffer.h"
#include "Kaesar/Renderer/UniformBuffer.h"
#include "Kaesar/Scene/Component.h"
#include "Kaesar/Renderer/RenderPass.h"

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
        static void UpdateLights(Scene& scene);
        static void BeginScene(const PerspectiveCamera& camera);
        static void RenderScene(Scene& scene);
        static void RenderEntityColor(const entt::entity& entity, TransformComponent& transform, MeshComponent& mesh);
        static void RenderEntityColor(const entt::entity& entity, TransformComponent& transform, MeshComponent& mesh, MaterialComponent& material);
        static void RenderEntityColor(const entt::entity& entity, TransformComponent& transform, MeshComponent& mesh, const std::shared_ptr<Shader>& shader);
        static void RenderEntityID(const entt::entity& entity, TransformComponent& transform, MeshComponent& mesh);
        static void EndScene();

        static void OnImGuiUpdate();

        static void OnViewportResize(uint32_t width, uint32_t height);

        static uint32_t GetTextureID(int index);
        static std::shared_ptr<FrameBuffer> GetMouseFB();
        static FramebufferSpecification GetMainFBSpec();

        static ShaderLibrary& GetShaderLibrary();

    public:
        struct CameraData
        {
            glm::mat4 viewProjection;
            glm::vec3 position;
        };

        struct TransformData
        {
            glm::mat4 transform;
            int id; // 记录实体的 ID，用于鼠标拾取
        };

        struct DirectionalLightData
        {
            glm::vec4 direction; // 平行光照射方向
            glm::vec4 color;
        };

        struct PointLightData
        {
            glm::vec4 position; // 点光源位置
            glm::vec4 color;
        };

        struct SpotLightData
        {
            glm::vec4 position; // 聚光灯位置
            glm::vec4 direction; // 聚光灯方向
            glm::vec4 color;
        };

        struct LightsParams
        {
            float pointLinear;
            float pointQuadratic;

            float spotLinear;
            float spotQuadratic;
            float innerCutOff;
            float outerCutOff;
        };

        struct ShadowData
        {
            glm::mat4 lightViewProjection;
        };

        struct DrawCall 
        {
            entt::entity id;
            TransformComponent transformComponent;
            MeshComponent meshComponent;
        };

        struct SceneData
        {
            CameraData cameraBuffer;
            TransformData transformBuffer;

            // lights
            float exposure; // 曝光度
            float gamma; // gamma 矫正
            float lightSize;
            DirectionalLightData directionalLightBuffer;
            PointLightData pointLightsBuffer[5];
            SpotLightData spotLightsBuffer[5];
            LightsParams lightsParamsBuffer[5];

            // shadow
            bool softShadow = true;
            float numPCF = 32;
            float numBlocker = 32;
            glm::mat4 lightProjection;
            glm::mat4 lightView;
            ShadowData shadowBuffer;

            std::shared_ptr<VertexArray> vertexArray;

            // Poisson-disk Distribution
            std::shared_ptr<Texture1D> distributionSampler0, distributionSampler1; // 用于 PCF 阴影采样

            // shader
            ShaderLibrary shaders;
            std::shared_ptr<Shader> basicShader, quadShader, mouseShader, lightShader, depthShader;

            // FrameBuffers
            std::shared_ptr<RenderPass> mainPass, shadowPass, mousePass, postPass;

            std::shared_ptr<UniformBuffer> cameraUniformBuffer, transformUniformBuffer, 
                                           lightsUniformBuffer, lightsParamsUniformBuffer,
                                           shadowUniformBuffer;

            glm::vec3 clearColor;
        };

        friend class ScenePanel;
        friend class SceneSerializer;
    };
}