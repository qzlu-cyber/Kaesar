#pragma once

#include "VertexArray.h"

#include <glm/glm.hpp>

namespace Kaesar {
    class RendererAPI
    {
    public:
        enum class API
        {
            None = 0, OpenGL = 1
        };

    public:
        virtual void Init() = 0; // 初始化

        virtual void SetClearColor(const glm::vec4& color) = 0; // 设置清屏颜色
        virtual void Clear() = 0; // 清屏
        virtual void ClearColor() = 0;
        virtual void EnableDepthTest() = 0; // 深度测试
        virtual void DisableDepthTest() = 0;

        virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) = 0; // 绘制索引顶点数组

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0; // 设置视口

        inline static API GetAPI() { return s_API; }

    private:
        static API s_API;
    };
}