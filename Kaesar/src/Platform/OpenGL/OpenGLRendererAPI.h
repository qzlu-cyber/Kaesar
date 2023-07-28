#pragma once

#include "Kaesar/Renderer/RendererAPI.h"

namespace Kaesar {
    class OpenGLRendererAPI : public RendererAPI
    {
        virtual void SetClearColor(const glm::vec4& color) override;
        virtual void Clear() override;
        virtual void DepthTest() override;

        virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
    };
}