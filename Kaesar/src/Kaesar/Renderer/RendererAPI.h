#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"

namespace Kaesar {
    class RendererAPI
    {
    public:
        enum class API
        {
            None = 0, OpenGL = 1
        };

    public:
        virtual void SetClearColor(const glm::vec4& color) = 0; // ����������ɫ
        virtual void Clear() = 0; // ����
        virtual void DepthTest() = 0; // ��Ȳ���

        virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) = 0; // ����������������

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0; // �����ӿ�

        inline static API GetAPI() { return s_API; }

    private:
        static API s_API;
    };
}