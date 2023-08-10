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
        static void RenderEntityID(const entt::entity& entity, TransformComponent& transform, MeshComponent& mesh);
        static void EndScene();

        static void OnViewportResize(uint32_t width, uint32_t height);

        static uint32_t GetTextureID(int index) { return s_Data->postProcessFB->GetColorAttachmentRendererID(index); }
        static std::shared_ptr<FrameBuffer> GetMouseFB() { return s_Data->mouseFB; }
        static FramebufferSpecification GetMainFBSpec() { return s_Data->mainFB->GetSpecification(); }

    private:
        struct CameraData
        {
            glm::mat4 viewProjection;
            glm::vec4 position;
        };

        struct TransformData
        {
            glm::mat4 transform;
            glm::vec4 lightPos;
        };

        struct SceneData
        {
            CameraData cameraBuffer;
            TransformData transformBuffer;
            std::shared_ptr<VertexArray> vertexArray;
            ShaderLibrary shaders;
            std::shared_ptr<Shader> basicShader, quadShader, mouseShader;
            std::shared_ptr<FrameBuffer> mainFB, mouseFB, postProcessFB;
            std::shared_ptr<UniformBuffer> cameraUniformBuffer, transformUniformBuffer;

            glm::vec3 clearColor;
        };

        static SceneData* s_Data;
    };
}