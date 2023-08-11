#pragma once

#include "Kaesar/Renderer/Shader.h"

#include <glad/glad.h>
#include <unordered_map>

namespace Kaesar {
    class OpenGLShader : public Shader
    {
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetInt(const std::string& name, int value) override;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;
		virtual void SetFloat(const std::string& name, float value) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

		virtual const std::string& GetName() const override;

		void UploadUniformInt(const std::string& name, int value);
		void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);

		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

		virtual std::vector<PushConstant> GetPushConstants() override { return m_PushConstants; };
		virtual std::vector<Sampler> GetSamplers() override { return m_Samplers; };

	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);

		void CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources);
		void CompileOrGetOpenGLBinaries();
		void Reflect(GLenum stage, const std::vector<uint32_t>& shaderData);

		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
	private:
		uint32_t m_RendererID;
		std::string m_FilePath;
		std::string m_Name;

		std::vector<PushConstant> m_PushConstants; // 着色器中的推送常量
		std::vector<Sampler> m_Samplers; // 着色器中的采样器

		std::unordered_map<GLenum, std::vector<uint32_t>> m_VulkanSPIRV; // Vulkan SPIR-V 二进制代码
		std::unordered_map<GLenum, std::vector<uint32_t>> m_OpenGLSPIRV; // OpenGL SPIR-V 二进制代码

		std::unordered_map<GLenum, std::string> m_OpenGLSourceCode; // OpenGL Shader 源代码
    };
}