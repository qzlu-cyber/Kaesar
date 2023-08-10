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
        KR_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "顶点缓冲区布局为空！");

        glBindVertexArray(m_RendererID); // 确保在绑定顶点缓冲区之前绑定顶点数组对象
        vertexBuffer->Bind();

        // 顶点缓冲区的布局
        const BufferLayout& layout = vertexBuffer->GetLayout();
        for (const auto& element : layout) {
            switch (element.Type)
            {
                case ShaderDataType::Float:
                case ShaderDataType::Float2:
                case ShaderDataType::Float3:
                case ShaderDataType::Float4:
                case ShaderDataType::Int:
                case ShaderDataType::Int2:
                case ShaderDataType::Int3:
                case ShaderDataType::Int4:
                case ShaderDataType::Bool:
                    {
                        // 设定顶点属性指针
                        glEnableVertexAttribArray(m_VertexBufferIndex);
                        glVertexAttribPointer(m_VertexBufferIndex,
                            element.GetComponentCount(),
                            ShaderDataTypeToOpenGLBaseType(element.Type),
                            element.Normalized ? GL_TRUE : GL_FALSE,
                            layout.GetStride(),
                            (const void*)element.Offset);
                        m_VertexBufferIndex++;
                        break;
                    }
                case ShaderDataType::Mat3:
                case ShaderDataType::Mat4:
                    {
                        uint8_t count = element.GetComponentCount();
                        for (uint8_t i = 0; i < count; i++)
                        {
                            glEnableVertexAttribArray(m_VertexBufferIndex);
                            glVertexAttribPointer(m_VertexBufferIndex,
                                count,
                                ShaderDataTypeToOpenGLBaseType(element.Type),
                                element.Normalized ? GL_TRUE : GL_FALSE,
                                layout.GetStride(),
                                (const void*)(sizeof(float) * count * i));
                            // 设置实例分割参数，告诉 OpenGL 在渲染时应该如何使用顶点数据。这里设置为 1，表示每个矩阵列都是一个独立的实例
                            glVertexAttribDivisor(m_VertexBufferIndex, 1);
                            m_VertexBufferIndex++;
                        }
                        break;
                    }
                default:
                    KR_CORE_ASSERT(false, "未定义的 ShaderType！");
            }
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

