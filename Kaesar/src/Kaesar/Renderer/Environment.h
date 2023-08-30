#pragma once

#include "Kaesar/Renderer/Shader.h"
#include "Kaesar/Renderer/Renderer.h"
#include "Kaesar/Renderer/Texture.h"
#include "Kaesar/Renderer/VertexArray.h"
#include "Kaesar/Renderer/FrameBuffer.h"

namespace Kaesar {
	class Environment
	{
	public:
		Environment(const std::shared_ptr<Texture2D>& hdri);

		void RenderBackground();
		void SetHDRTexture(const std::shared_ptr<Texture2D>& hdri) { m_HDRSkyMap = hdri; }
		void SetViewProjection(const glm::mat4& view, const glm::mat4& projection) { m_View = view; m_Projection = projection; }

		void BindIrradianceMap(uint32_t slot);

	private:
		void SetupCube();
		void SetupFrameBuffer();

		void RenderCube();

	private:
		std::shared_ptr<Texture2D> m_HDRSkyMap;
		std::shared_ptr<Shader> m_EquirectangularToCube, m_BackgroundShader, m_IrradianceConvShader;
		std::shared_ptr<VertexArray> m_CubeVAO;
		std::shared_ptr<FrameBuffer> m_CaptureFBO, m_IrradianceFBO;
		glm::mat4 m_View, m_Projection;
	};
}