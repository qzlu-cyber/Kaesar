#include "krpch.h"
#include "FrameBuffer.h"

#include "Kaesar/Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace Kaesar {
    std::shared_ptr<FrameBuffer> FrameBuffer::Create(const FramebufferSpecification& fspc)
    {
        switch (RendererAPI::GetAPI())
        {
            case RendererAPI::API::None:   KR_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::OpenGL: return std::make_shared<OpenGLFrameBuffer>(fspc);
            default: break;
        }

        KR_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}