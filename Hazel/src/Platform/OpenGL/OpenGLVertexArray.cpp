#include "hzpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Hazel {
    static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
    {
        switch (type)
        {
            case Hazel::ShaderDataType::Float:    return GL_FLOAT;
            case Hazel::ShaderDataType::Float2:   return GL_FLOAT;
            case Hazel::ShaderDataType::Float3:   return GL_FLOAT;
            case Hazel::ShaderDataType::Float4:   return GL_FLOAT;
            case Hazel::ShaderDataType::Mat3:     return GL_FLOAT;
            case Hazel::ShaderDataType::Mat4:     return GL_FLOAT;
            case Hazel::ShaderDataType::Int:      return GL_INT;
            case Hazel::ShaderDataType::Int2:     return GL_INT;
            case Hazel::ShaderDataType::Int3:     return GL_INT;
            case Hazel::ShaderDataType::Int4:     return GL_INT;
            case Hazel::ShaderDataType::Bool:     return GL_BOOL;
        }

        HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
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
        HZ_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "顶点缓冲区布局为空！");

        glBindVertexArray(m_RendererID); // 确保在绑定顶点缓冲区之前绑定顶点数组对象
        vertexBuffer->Bind();

        // 顶点缓冲区的布局
        uint32_t index = 0;
        const BufferLayout& layout = vertexBuffer->GetLayout();
        for (const auto& element : layout) {
            // 设定顶点属性指针
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
        glBindVertexArray(m_RendererID); // 确保在绑定顶点缓冲区之前绑定顶点数组对象
        indexBuffer->Bind();

        m_IndexBuffer = indexBuffer;
    }
}

