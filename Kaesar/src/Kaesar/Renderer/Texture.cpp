#include "krpch.h"
#include "Texture.h"

#include "Kaesar/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture2D.h"

namespace Kaesar {
    std::shared_ptr<Texture2D> Texture2D::Create(const std::string& filepath, bool vertical)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:   KR_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(filepath, vertical);
            default: break;
        }

        KR_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    void Texture::BindTexture(uint32_t rendererID, uint32_t slot)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:   KR_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            case RendererAPI::API::OpenGL: OpenGLTexture2D::BindTexture(rendererID, slot);
            default: break;
        }
    }
}