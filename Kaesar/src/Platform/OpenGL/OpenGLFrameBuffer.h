#pragma once

#include "Kaesar/Renderer/FrameBuffer.h"

namespace Kaesar {
	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(const FramebufferSpecification& fspc);
		~OpenGLFrameBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void Invalidate() override;

		virtual void Resize(uint32_t width, uint32_t height) override;

		inline virtual uint32_t GetColorAttachmentRendererID() const override { return m_ColorAttachment; };

		inline virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; };

	private:
		unsigned int m_RendererID;
		unsigned int m_ColorAttachment;
		unsigned int m_DepthAttachment;

		FramebufferSpecification m_Specification;
	};
}
