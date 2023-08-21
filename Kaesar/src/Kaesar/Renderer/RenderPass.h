#pragma once

#include "Kaesar/Renderer/FrameBuffer.h"

namespace Kaesar {
	// 用于描述一个渲染通道的规格
	struct RenderPassSpecification
	{
		std::shared_ptr<FrameBuffer> TargetFrameBuffer; // 渲染通道的目标帧缓冲
	};

	class RenderPass 
	{
	public:
		RenderPass(const RenderPassSpecification& spec);
		~RenderPass() = default;

		uint32_t GetFrameBufferTextureID(uint32_t slot = 0);

		void BindTargetFrameBuffer();
		void UnbindTargetFrameBuffer();

		static std::shared_ptr<RenderPass> Create(const RenderPassSpecification& spec);
		const RenderPassSpecification& GetSpecification() const { return m_Specification; }

	private:
		RenderPassSpecification m_Specification;
	};
}