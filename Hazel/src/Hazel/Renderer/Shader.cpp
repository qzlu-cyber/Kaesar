#include "hzpch.h"
#include "Shader.h"

#include <glad/glad.h>

namespace Hazel {

    ShaderProgramSource Shader::ParseShader(const std::string& filePath)
    {
        std::ifstream stream(filePath); // ��ȡ�ļ���
        std::string line; // �洢ÿ������
        std::stringstream ss[2]; // �洢���� shader ����

        enum class ShaderType // ���ö�������
        {
            NONE = -1, VERTEX = 0, FRAGMENT = 1
        };

        ShaderType type = ShaderType::NONE; // ��ʼ����������

        while (getline(stream, line)) { // ���ж�ȡ
            if (line.find("#shader") != std::string::npos) {
                if (line.find("vertex") != std::string::npos) {
                    type = ShaderType::VERTEX;
                }
                else if (line.find("fragment") != std::string::npos) {
                    type = ShaderType::FRAGMENT;
                }
            }
            else
            {
                ss[(int)type] << line << "\n"; // ��ÿһ�е�������ӵ���Ӧ�� stringstream ��
            }
        }

        return { ss[0].str(), ss[1].str() };
    }

    unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
    {
        unsigned int shaderID = glCreateShader(type);
        const char* src = source.c_str();

        glShaderSource(shaderID, 1, &src, NULL);

        glCompileShader(shaderID);

        // Error handling
        int result;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result); // �������Ƿ�ɹ�����һ�������� shader ����� ID���ڶ���������Ҫ��ѯ��״̬�������������Ǵ洢��ѯ����ı���
        if (result == GL_FALSE) {
            GLint maxLength = 0;
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(shaderID, maxLength, &maxLength, &infoLog[0]);

            // We don't need the shader anymore.
            glDeleteShader(shaderID);

            HZ_CORE_ERROR("{0}", infoLog.data());
            HZ_CORE_ASSERT(false, "��ɫ������ʧ�ܣ�");
        }

        return shaderID;
    }

    unsigned int Shader::AttachAndLinkShader(unsigned int vertexShader, unsigned int fragmentShader, uint32_t shaderProgram)
    {
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, (int*)&isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &infoLog[0]);

            // We don't need the program anymore.
            glDeleteProgram(shaderProgram);
            // Don't leak shaders either.
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            HZ_CORE_ERROR("{0}", infoLog.data());
            HZ_CORE_ASSERT(false, "��ɫ������ʧ�ܣ�");
            return 0;
        }

        // Always detach shaders after a successful link.
        glDetachShader(shaderProgram, vertexShader);
        glDetachShader(shaderProgram, fragmentShader);
    }

    void Shader::SetMatrix(const std::string& name, const glm::mat4& matrix)
    {
        glUniformMatrix4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
    }

    Shader::Shader(const std::string& filePath)
    {
        // ������ɫ������
        unsigned int vertexShader, fragmentShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        // ��ȡ shader Դ����
        ShaderProgramSource source = ParseShader(filePath);

        // ���� shader
        unsigned int vs = Shader::CompileShader(GL_VERTEX_SHADER, source.VertexShader);
        unsigned int fs = Shader::CompileShader(GL_FRAGMENT_SHADER, source.FragmentShader);

        m_RendererID = glCreateProgram();

        Shader::AttachAndLinkShader(vs, fs, m_RendererID);
    }

    Shader::~Shader()
    {
        glDeleteProgram(m_RendererID);
    }

    void Shader::Bind() const
    {
        glUseProgram(m_RendererID);
    }

    void Shader::Unbind() const
    {
        glUseProgram(0);
    }

}