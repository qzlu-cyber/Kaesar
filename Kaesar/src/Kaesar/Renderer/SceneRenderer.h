#pragma once

#include "Kaesar/Renderer/Camera/PerspectiveCamera.h"
#include "Kaesar/Renderer/VertexArray.h"
#include "Kaesar/Renderer/Buffer.h"
#include "Kaesar/Renderer/Shader.h"
#include "Kaesar/Renderer/FrameBuffer.h"
#include "Kaesar/Renderer/UniformBuffer.h"
#include "Kaesar/Scene/Component.h"

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
        static void BeginScene(const PerspectiveCamera& camera);
        static void RenderScene(Scene& scene);
        static void RenderEntityColor(const entt::entity& entity, TransformComponent& transform, MeshComponent& mesh);
        static void RenderEntityColor(const entt::entity& entity, TransformComponent& transform, MeshComponent& mesh, MaterialComponent& material);
        static void RenderEntityColor(const entt::entity& entity, TransformComponent& transform, MeshComponent& mesh, const std::shared_ptr<Shader>& shader);
        static void RenderEntityID(const entt::entity& entity, TransformComponent& transform, MeshComponent& mesh);
        static void EndScene();

        static void OnImGuiUpdate();

        static void OnViewportResize(uint32_t width, uint32_t height);

        static void SetExposure(float exposure) { s_Data->exposure = exposure; }
        static void SetGamma(float gamma) { s_Data->gamma = gamma; }

        static uint32_t GetTextureID(int index) { return s_Data->postProcessFB->GetColorAttachmentRendererID(index); }
        static std::shared_ptr<FrameBuffer> GetMouseFB() { return s_Data->mouseFB; }
        static FramebufferSpecification GetMainFBSpec() { return s_Data->mainFB->GetSpecification(); }

    private:
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
            glm::vec3 direction; // 平行光照射方向
            glm::vec3 ambient;
            glm::vec3 diffuse;
            glm::vec3 specular;
        };

        struct PointLightData
        {
            glm::vec3 position; // 点光源位置
            glm::vec3 ambient;
            glm::vec3 diffuse;
            glm::vec3 specular;
        };

        struct SpotLightData
        {
            glm::vec3 position; // 聚光灯位置
            glm::vec3 direction; // 聚光灯方向
            glm::vec3 ambient;
            glm::vec3 diffuse;
            glm::vec3 specular;
        };

        struct LightsParams
        {
            float dirIntensity;

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

        struct SceneData
        {
            CameraData cameraBuffer;
            TransformData transformBuffer;

            float exposure; // 曝光度
            float gamma; // gamma 矫正
            DirectionalLightData directionalLightBuffer;
            PointLightData pointLightBuffer;
            SpotLightData spotLightBuffer;
            LightsParams lightsParamsBuffer;

            glm::mat4 lightProjection;
            glm::mat4 lightView;
            ShadowData shadowBuffer;

            std::shared_ptr<VertexArray> vertexArray;

            ShaderLibrary shaders;

            std::shared_ptr<Shader> basicShader, quadShader, mouseShader, lightShader, depthShader;

            std::shared_ptr<FrameBuffer> mainFB, mouseFB, postProcessFB, shadowFB;

            std::shared_ptr<UniformBuffer> cameraUniformBuffer, transformUniformBuffer, 
                                           lightsUniformBuffer, lightsParamsUniformBuffer,
                                           shadowUniformBuffer;

            glm::vec3 clearColor;
        };

        static SceneData* s_Data;

        friend class ScenePanel;
        friend class SceneSerializer;
    };
}