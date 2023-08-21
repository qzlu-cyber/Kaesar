#include "krpch.h"
#include "RenderPass.h"

namespace Kaesar {
	RenderPass::RenderPass(const RenderPassSpecification& spec)
	{
		m_Specification = spec;
	}

	std::shared_ptr<RenderPass> RenderPass::Create(const RenderPassSpecification& spec)
	{
		return std::make_shared<RenderPass>(spec);
	}

	uint32_t RenderPass::GetFrameBufferTextureID(uint32_t slot)
	{
		return m_Specification.TargetFrameBuffer->GetColorAttachmentRendererID(slot);
	}

	void RenderPass::BindTargetFrameBuffer()
	{
		m_Specification.TargetFrameBuffer->Bind();
	}

	void RenderPass::UnbindTargetFrameBuffer()
	{
		m_Specification.TargetFrameBuffer->Unbind();
	}
}