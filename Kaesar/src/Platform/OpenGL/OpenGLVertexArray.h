#pragma once

#include "Kaesar/Renderer/VertexArray.h"

namespace Kaesar {
    class OpenGLVertexArray : public VertexArray
    {
    public:
        OpenGLVertexArray();
        virtual ~OpenGLVertexArray();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
        virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

        virtual std::vector<std::shared_ptr<VertexBuffer>> GetVertexBuffers() const override { return m_VertexBuffers; };
        virtual std::shared_ptr<IndexBuffer> GetIndexBuffer() const override { return m_IndexBuffer; };

    private:
        uint32_t m_RendererID;
        uint32_t m_VertexBufferIndex = 0; // �������Ե�����
        std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers; // ������㻺�����б�
        std::shared_ptr<IndexBuffer> m_IndexBuffer; // ����������
    };
}