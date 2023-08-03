#pragma once

#include <memory>

namespace Kaesar {
    struct FramebufferSpecification
    {
        uint32_t Width = 0, Height = 0;
        uint32_t Samples; // 多重采样样本数
        bool SwapChainTarget = false;
    };

    class FrameBuffer
    {
    public:
        ~FrameBuffer() = default;

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void BlitMultiSample(unsigned int readFrameBuffer, unsigned int drawFrameBuffer) const = 0;

        virtual void Invalidate() = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;

        virtual uint32_t GetRendererID() const = 0;
        virtual uint32_t GetColorAttachmentRendererID() const = 0;

        virtual const FramebufferSpecification& GetSpecification() const = 0;

    public:
        static std::shared_ptr<FrameBuffer> Create(const FramebufferSpecification& fspc);
    };
}