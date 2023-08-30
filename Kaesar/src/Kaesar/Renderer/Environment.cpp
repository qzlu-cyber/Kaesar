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

		SetupCube();

		SetupFrameBuffer();

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
		m_CaptureFBO->Bind();

		m_EquirectangularToCube->Bind();
		m_EquirectangularToCube->SetMat4("camera.u_Projection", captureProjection);

		m_HDRSkyMap->Bind(0);
		for (unsigned int i = 0; i < 6; ++i)
		{
			m_EquirectangularToCube->SetMat4("camera.u_View", captureViews[i]);
			m_CaptureFBO->BindCubemapFace(i);
			RenderCommand::Clear();
			RenderCube();
		}

		m_EquirectangularToCube->Unbind();
		m_CaptureFBO->Unbind();

		// 将立方体贴图转换为辐照度贴图
		m_IrradianceFBO->Bind();

		m_IrradianceConvShader->Bind();
		m_IrradianceConvShader->SetMat4("camera.u_Projection", captureProjection);

		Texture2D::BindTexture(m_CaptureFBO->GetColorAttachmentRendererID(), 0);

		for (unsigned int i = 0; i < 6; ++i)
		{
			m_IrradianceConvShader->SetMat4("camera.u_View", captureViews[i]);
			m_IrradianceFBO->BindCubemapFace(i);
			RenderCommand::Clear();
			RenderCube();
		}
		m_IrradianceConvShader->Unbind();
		m_IrradianceFBO->Unbind();
    }

	void Environment::RenderCube()
	{
		m_CubeVAO->Bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	void Environment::RenderBackground()
	{
		m_BackgroundShader->Bind();

		Texture2D::BindTexture(m_CaptureFBO->GetColorAttachmentRendererID(), 0);

		m_BackgroundShader->SetMat4("camera.u_View", m_View);
		m_BackgroundShader->SetMat4("camera.u_Projection", m_Projection);

		RenderCube();
	}

	void Environment::BindIrradianceMap(uint32_t slot)
	{
		Texture2D::BindTexture(m_IrradianceFBO->GetColorAttachmentRendererID(), slot);
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

	void Environment::SetupFrameBuffer()
	{
		FramebufferSpecification spec;
		spec.Attachments = { FramebufferTextureFormat::Cubemap, FramebufferTextureFormat::DEPTH24STENCIL8 };
		spec.Width = 2048;
		spec.Height = 2048;
		spec.Samples = 1;
		spec.ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

		m_CaptureFBO = FrameBuffer::Create(spec);

		spec.Width = 32;
		spec.Height = 32;
		m_IrradianceFBO = FrameBuffer::Create(spec);
	}
}