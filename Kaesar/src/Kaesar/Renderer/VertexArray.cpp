#include "Platform/OpenGL/OpenGLVertexArray.h"

#include "krpch.h"
#include "VertexArray.h"
#include "Renderer.h"

namespace Kaesar {
    VertexArray* VertexArray::Create()
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:             KR_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::OpenGL:           return new OpenGLVertexArray();
            default:
                break;
        }

        KR_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}