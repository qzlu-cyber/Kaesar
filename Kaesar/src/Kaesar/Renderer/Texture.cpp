#include "krpch.h"
#include "Texture.h"

#include "Kaesar/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture2D.h"
#include "Platform/OpenGL/OpenGLTexture1D.h"

namespace Kaesar {
    std::shared_ptr<Texture2D> Texture2D::Create(const std::string& filepath, bool vertical, bool sRGB)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:   KR_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(filepath, vertical, sRGB);
            default: break;
        }

        KR_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    std::shared_ptr<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, const unsigned char* data, bool vertical, bool sRGB)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:   KR_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture2D>(width, height, data, vertical, sRGB);
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

    std::shared_ptr<Texture1D> Texture1D::Create(uint32_t size)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:   KR_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture1D>(size);
            default: break;
        }

        KR_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
    std::shared_ptr<Texture1D> Texture1D::Create(uint32_t size, void* data)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:   KR_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::OpenGL: return std::make_shared<OpenGLTexture1D>(size, data);
            default: break;
        }

        KR_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}