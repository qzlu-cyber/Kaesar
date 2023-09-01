#include "krpch.h"
#include "Environment.h"

#include "Kaesar/Renderer/RenderCommand.h"

#include <glad/glad.h>

namespace Kaesar {
    Environment::Environment(const std::shared_ptr<Texture2D>& hdri)
        : m_HDRSkyMap(hdri)
    {
		m_BackgroundShader		= Shader::Create("assets/shaders/BackgroundSky.glsl");
		m_EquirectangularToCube = Shader::Create("assets/shaders/EquirectangularToCube.glsl");
		m_IrradianceConvShader  = Shader::Create("assets/shaders/IrradianceConvolution.glsl");
		m_PrefilterShader		= Shader::Create("assets/shaders/Prefilter.glsl");
		m_BRDFLutShader			= Shader::Create("assets/shaders/BRDFLut.glsl");

		m_BackgroundShader->Bind();
		m_BackgroundShader->SetFloat("pc.intensity", 1.0);
		m_BackgroundShader->Unbind();

		SetupCube();
		SetupQuad();

		//SetupFrameBuffer();

		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] = {
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		// 将 HDR 贴图转换为立方体贴图
		//m_CaptureFBO->Bind();
		unsigned int captureFBO;
		unsigned int captureRBO;
		glGenFramebuffers(1, &captureFBO);
		glGenRenderbuffers(1, &captureRBO);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 2048, 2048);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

		glGenTextures(1, &m_EnvCubemap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_EnvCubemap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 2048, 2048, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		m_EquirectangularToCube->Bind();
		m_EquirectangularToCube->SetMat4("camera.u_Projection", captureProjection);

		m_HDRSkyMap->Bind(0);

		glViewport(0, 0, 2048, 2048); // 将视口设置为适合立方体贴图的尺寸
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

		for (unsigned int i = 0; i < 6; ++i)
		{
			m_EquirectangularToCube->SetMat4("camera.u_View", captureViews[i]);
			//m_CaptureFBO->BindCubemapFace(i);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_EnvCubemap, 0);
			RenderCommand::Clear();
			RenderCube();
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		m_EquirectangularToCube->Unbind();
		//m_CaptureFBO->Unbind();

		glBindTexture(GL_TEXTURE_CUBE_MAP, m_EnvCubemap);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		// 将立方体贴图转换为辐照度贴图
		//m_IrradianceFBO->Bind();
		glGenTextures(1, &m_IrradianceMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_IrradianceMap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

		m_IrradianceConvShader->Bind();
		m_IrradianceConvShader->SetMat4("camera.u_Projection", captureProjection);

		Texture2D::BindTexture(m_EnvCubemap, 0);

		glViewport(0, 0, 32, 32);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

		for (unsigned int i = 0; i < 6; ++i)
		{
			m_IrradianceConvShader->SetMat4("camera.u_View", captureViews[i]);
			//m_IrradianceFBO->BindCubemapFace(i);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_IrradianceMap, 0);
			RenderCommand::Clear();
			RenderCube();
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		m_IrradianceConvShader->Unbind();
		//m_IrradianceFBO->Unbind();

		// 创建预过滤 HDR 环境贴图
		glGenTextures(1, &m_PrefilterMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_PrefilterMap);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		m_PrefilterShader->Bind();
		m_PrefilterShader->SetMat4("camera.u_Projection", captureProjection);
		Texture2D::BindTexture(m_EnvCubemap, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

		unsigned int maxMipLevels = 5;
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			// resize framebuffer according to mip-level size.
			unsigned int mipWidth = 512 * std::pow(0.5, mip);
			unsigned int mipHeight = 512 * std::pow(0.5, mip);
			glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(maxMipLevels - 1);
			m_PrefilterShader->SetFloat("pc.roughness", roughness);
			for (unsigned int i = 0; i < 6; ++i)
			{
				m_PrefilterShader->SetMat4("camera.u_View", captureViews[i]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_PrefilterMap, mip);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				RenderCube();
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		m_PrefilterShader->Unbind();

		glGenTextures(1, &m_BrdfLUTTexture);

		// pre-allocate enough memory for the LUT texture.
		glBindTexture(GL_TEXTURE_2D, m_BrdfLUTTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BrdfLUTTexture, 0);

		glViewport(0, 0, 512, 512);
		m_BRDFLutShader->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		RenderQuad();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		m_BRDFLutShader->Unbind();
    }

	void Environment::RenderCube()
	{
		m_CubeVAO->Bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	void Environment::RenderQuad()
	{
		m_QuadVAO->Bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	void Environment::RenderBackground()
	{
		m_BackgroundShader->Bind();

		//Texture2D::BindTexture(m_CaptureFBO->GetColorAttachmentRendererID(), 0);
		Texture2D::BindTexture(m_EnvCubemap, 0);

		m_BackgroundShader->SetMat4("camera.u_View", m_View);
		m_BackgroundShader->SetMat4("camera.u_Projection", m_Projection);

		RenderCube();
	}

	void Environment::BindIrradianceMap(uint32_t slot)
	{
		Texture2D::BindTexture(m_IrradianceMap, slot);
	}

	void Environment::BindPreFilterMap(uint32_t slot)
	{
		Texture2D::BindTexture(m_PrefilterMap, slot);
	}

	void Environment::BindBRDFMap(uint32_t slot)
	{
		Texture2D::BindTexture(m_BrdfLUTTexture, slot);
	}

	void Environment::SetIntensity(float intensity)
	{
		m_BackgroundShader->Bind();
		m_BackgroundShader->SetFloat("pc.intensity", intensity);
		m_BackgroundShader->Unbind();
	}

	void Environment::SetupCube()
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			 // bottom face
			 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			  1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 // top face
			 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			  1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			  1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			  1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		m_CubeVAO = VertexArray::Create();
		std::shared_ptr<VertexBuffer> cubeVBO = VertexBuffer::Create(vertices, sizeof(vertices));

		BufferLayout cubeLayout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float2, "a_TexCoords" }

		};

		cubeVBO->SetLayout(cubeLayout);
		m_CubeVAO->AddVertexBuffer(cubeVBO);
	}

	void Environment::SetupQuad()
	{
		m_QuadVAO = VertexArray::Create();

		float quad[] = {
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,   // top right
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom right
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,   // bottom left
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f    // top left 
		};

		std::shared_ptr<VertexBuffer> quadVBO = VertexBuffer::Create(quad, sizeof(quad));
		BufferLayout quadLayout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoords" },
		};
		quadVBO->SetLayout(quadLayout);
		m_QuadVAO->AddVertexBuffer(quadVBO);

		unsigned int quadIndices[] = {
			0, 3, 1, // first triangle
			1, 3, 2  // second triangle
		};
		std::shared_ptr<IndexBuffer> quadEBO = IndexBuffer::Create(quadIndices, sizeof(quadIndices) / sizeof(uint32_t));
		m_QuadVAO->SetIndexBuffer(quadEBO);
	}

	void Environment::SetupFrameBuffer()
	{
		FramebufferSpecification spec;
		spec.Attachments = { FramebufferTextureFormat::Cubemap, FramebufferTextureFormat::DEPTH24STENCIL8 };
		spec.Width = 2048;
		spec.Height = 2048;
		spec.Samples = 1;
		spec.ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

		//m_CaptureFBO = FrameBuffer::Create(spec);

		spec.Width = 32;
		spec.Height = 32;
		//m_IrradianceFBO = FrameBuffer::Create(spec);
	}
}