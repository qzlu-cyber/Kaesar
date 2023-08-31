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
		uint32_t m_RendererID; // ֡�������� ID
		std::vector<uint32_t> m_ColorAttachments; // ������ɫ������ ID
		uint32_t m_DepthAttachment; // ��ȸ����� ID
		uint32_t m_CubemapAttachment = 0; // ��������ͼ�� ID

		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications; // ������ɫ�����Ĺ��
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None; // ��ȸ����Ĺ��
		FramebufferTextureSpecification m_CubeMapAttachmentSpecification = FramebufferTextureFormat::None; // ��������ͼ�����Ĺ��
	};
}
