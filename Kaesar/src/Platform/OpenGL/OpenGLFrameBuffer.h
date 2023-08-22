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

		virtual void BindCubemapFace(uint32_t index) const override;

		virtual void Invalidate() override;

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		inline virtual uint32_t GetRendererID() const override { return m_RendererID; };
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override;
		virtual uint32_t GetDepthAttachmentRendererID() const override { return m_DepthAttachment; }
		virtual void OpenGLFrameBuffer::ClearAttachment(uint32_t attachmentIndex, int value) override;

		inline virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; };

	private:
		unsigned int m_RendererID; // 帧缓冲对象的 ID
		std::vector<uint32_t> m_ColorAttachments; // 所有颜色附件的 ID
		unsigned int m_DepthAttachment; // 深度附件的 ID
		unsigned int m_CubemapAttachment; // 立方体贴图附件的 ID

		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications; // 所有颜色附件的规格
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None; // 深度附件的规格
		FramebufferTextureSpecification m_CubeMapAttachmentSpecification = FramebufferTextureFormat::None; // 立方体贴图附件的规格
	};
}
