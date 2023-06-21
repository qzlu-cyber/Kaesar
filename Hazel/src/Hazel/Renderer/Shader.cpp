#include "hzpch.h"
#include "Shader.h"

#include <glad/glad.h>

namespace Hazel {

    ShaderProgramSource Shader::ParseShader(const std::string& filePath)
    {
        std::ifstream stream(filePath); // 读取文件流
        std::string line; // 存储每行数据
        std::stringstream ss[2]; // 存储两种 shader 代码

        enum class ShaderType // 设置顶点类型
        {
            NONE = -1, VERTEX = 0, FRAGMENT = 1
        };

        ShaderType type = ShaderType::NONE; // 初始化顶点类型

        while (getline(stream, line)) { // 逐行读取
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
                ss[(int)type] << line << "\n"; // 将每一行的内容添加到对应的 stringstream 中
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
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result); // 检查编译是否成功，第一个参数是 shader 对象的 ID，第二个参数是要查询的状态，第三个参数是存储查询结果的变量
        if (result == GL_FALSE) {
            GLint maxLength = 0;
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(shaderID, maxLength, &maxLength, &infoLog[0]);

            // We don't need the shader anymore.
            glDeleteShader(shaderID);

            HZ_CORE_ERROR("{0}", infoLog.data());
            HZ_CORE_ASSERT(false, "着色器编译失败！");
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
            HZ_CORE_ASSERT(false, "着色器链接失败！");
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
        // 创建着色器对象
        unsigned int vertexShader, fragmentShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        // 获取 shader 源代码
        ShaderProgramSource source = ParseShader(filePath);

        // 编译 shader
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