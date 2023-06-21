#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Hazel {
	struct ShaderProgramSource // �洢���� shader ��Դ����. C++ ����������ֵ
	{
		std::string VertexShader;
		std::string FragmentShader;
	};

	class Shader
	{
	public:
		Shader(const std::string& filePath);
		~Shader();

		void Bind() const;
		void Unbind() const;

		ShaderProgramSource ParseShader(const std::string& filePath);
		unsigned int CompileShader(unsigned int type, const std::string& source);
		unsigned int AttachAndLinkShader(unsigned int vertexShader, unsigned int fragmentShader, uint32_t shaderProgram);
		void SetMatrix(const std::string& name, const glm::mat4& matrix);

	private:
		uint32_t m_RendererID;
	};
}