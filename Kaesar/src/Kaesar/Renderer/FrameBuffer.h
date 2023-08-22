#pragma once

#include <memory>
#include <glm/glm.hpp>

namespace Kaesar {
    // 帧缓冲纹理附件的格式
    enum class FramebufferTextureFormat
    {
        None = 0,

        // Color
        RGBA8,
        RED_INTEGER,
        RGBA16F,

        Cubemap,

        // Depth/stencil
        DEPTH24STENCIL8,
        DEPTH32
    };

    struct FramebufferTextureSpecification
    {
        FramebufferTextureSpecification() = default;
        FramebufferTextureSpecification(FramebufferTextureFormat format)
            : TextureFormat(format) {}

        FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
        // TODO: filtering/wrap
    };

    // 所有帧缓冲附件的规格的集合
    struct FramebufferAttachmentSpecification
    {
        FramebufferAttachmentSpecification() = default;
        FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
            : Attachments(attachments) {}

        std::vector<FramebufferTextureSpecification> Attachments;
    };

    // 创建帧缓冲的规格
    struct FramebufferSpecification
    {
        uint32_t Width = 0, Height = 0;
        uint32_t Samples; // 多重采样样本数
        FramebufferAttachmentSpecification Attachments;
        glm::vec4 ClearColor;
    };

    class FrameBuffer
    {
    public:
        ~FrameBuffer() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void BindCubemapFace(uint32_t index) const = 0;

        virtual void Invalidate() = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;

        virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

        virtual uint32_t GetRendererID() const = 0;
        virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
        virtual uint32_t GetDepthAttachmentRendererID() const = 0;
        virtual void ClearAttachment(uint32_t index, int value) = 0;

        virtual const FramebufferSpecification& GetSpecification() const = 0;

    public:
        static std::shared_ptr<FrameBuffer> Create(const FramebufferSpecification& fspc);
    };
}