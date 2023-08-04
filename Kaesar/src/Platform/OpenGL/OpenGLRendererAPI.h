#pragma once

#include "Kaesar/Renderer/RendererAPI.h"

namespace Kaesar {
    class OpenGLRendererAPI : public RendererAPI
    {
        virtual void Init() override;
        virtual std::string GetRendererInfo() override;
        virtual void SetClearColor(const glm::vec4& color) override;
        virtual void Clear() override;
        virtual void ClearColor() override;
        virtual void EnableDepthTest() override;
        virtual void DisableDepthTest() override;

        virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    };
}