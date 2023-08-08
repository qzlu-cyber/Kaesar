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

		virtual void BlitMultiSample(unsigned int readFrameBuffer, unsigned int drawFrameBuffer) const override;

		virtual void Invalidate() override;

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		inline virtual uint32_t GetRendererID() const override { return m_RendererID; };
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override
		{
			KR_CORE_ASSERT(index < m_ColorAttachments.size(), "��������С��֡������ɫ�����ĸ�����");
			return m_ColorAttachments[index];
		};
		virtual void OpenGLFrameBuffer::ClearAttachment(uint32_t attachmentIndex, int value) override;

		inline virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; };

	private:
		unsigned int m_RendererID; // ֡�������� ID
		std::vector<uint32_t> m_ColorAttachments; // ������ɫ������ ID
		unsigned int m_DepthAttachment; // ��ȸ����� ID

		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications; // ������ɫ�����Ĺ��
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None; // ��ȸ����Ĺ��
	};
}
