#include "krpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Kaesar {
    static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
    {
        switch (type)
        {
            case Kaesar::ShaderDataType::Float:    return GL_FLOAT;
            case Kaesar::ShaderDataType::Float2:   return GL_FLOAT;
            case Kaesar::ShaderDataType::Float3:   return GL_FLOAT;
            case Kaesar::ShaderDataType::Float4:   return GL_FLOAT;
            case Kaesar::ShaderDataType::Mat3:     return GL_FLOAT;
            case Kaesar::ShaderDataType::Mat4:     return GL_FLOAT;
            case Kaesar::ShaderDataType::Int:      return GL_INT;
            case Kaesar::ShaderDataType::Int2:     return GL_INT;
            case Kaesar::ShaderDataType::Int3:     return GL_INT;
            case Kaesar::ShaderDataType::Int4:     return GL_INT;
            case Kaesar::ShaderDataType::Bool:     return GL_BOOL;
        }

        KR_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }

    OpenGLVertexArray::OpenGLVertexArray()
    {
        glCreateVertexArrays(1, &m_RendererID);
    }

    OpenGLVertexArray::~OpenGLVertexArray()
    {
        glDeleteVertexArrays(1, &m_RendererID);
    }

    void OpenGLVertexArray::Bind() const
    {
        glBindVertexArray(m_RendererID);
    }

    void OpenGLVertexArray::Unbind() const
    {
        glBindVertexArray(0);
    }

    void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
    {
        KR_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "���㻺��������Ϊ�գ�");

        glBindVertexArray(m_RendererID); // ȷ���ڰ󶨶��㻺����֮ǰ�󶨶����������
        vertexBuffer->Bind();

        // ���㻺�����Ĳ���
        uint32_t index = 0;
        const BufferLayout& layout = vertexBuffer->GetLayout();
        for (const auto& element : layout) {
            // �趨��������ָ��
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(
                index,
                element.GetComponentCount(),
                ShaderDataTypeToOpenGLBaseType(element.Type),
                element.Normalized ? GL_TRUE : GL_FALSE,
                layout.GetStride(),
                (const void*)(element.Offset)
            );
            index++;
        }

        m_VertexBuffers.push_back(vertexBuffer);
    }

    void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
    {
        glBindVertexArray(m_RendererID); // ȷ���ڰ󶨶��㻺����֮ǰ�󶨶����������
        indexBuffer->Bind();

        m_IndexBuffer = indexBuffer;
    }
}
