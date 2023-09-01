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
		void BindPreFilterMap(uint32_t slot);
		void BindBRDFMap(uint32_t slot);

		void SetIntensity(float intensity);

		std::string GetPath() const { return m_HDRSkyMap->GetPath(); }
		uint32_t GetBackgroundTextureID() const;

	private:
		void SetupCube();
		void SetupQuad();
		void SetupFrameBuffer();

		void RenderCube();
		void RenderQuad();

	private:
		unsigned int m_EnvCubemap;
		unsigned int m_BrdfLUTTexture;
		unsigned int m_PrefilterMap;
		unsigned int m_IrradianceMap;
		std::shared_ptr<Texture2D> m_HDRSkyMap;
		std::shared_ptr<Shader> m_EquirectangularToCube, m_BackgroundShader, m_IrradianceConvShader, m_PrefilterShader, m_BRDFLutShader;
		std::shared_ptr<VertexArray> m_CubeVAO, m_QuadVAO;
		std::shared_ptr<FrameBuffer> m_IrradianceFBO, m_PrefilterFBO;
		glm::mat4 m_View, m_Projection;
	};
}