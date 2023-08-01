#include "krpch.h"
#include "OpenGLShader.h"

namespace Kaesar {
    static GLenum ShaderTypeFromString(const std::string& type)
    {
        if (type == "vertex")
            return GL_VERTEX_SHADER;
        if (type == "fragment" || type == "pixel")
            return GL_FRAGMENT_SHADER;

        KR_CORE_ASSERT(false, "Unknown shader type!");
        return 0;
    }

    OpenGLShader::OpenGLShader(const std::string& filepath)
    {
        std::string source = ReadFile(filepath);
        std::unordered_map<GLenum, std::string> shaderSources = PreProcess(source);
        Compile(shaderSources);

        // 获取 shader 名称
        auto lastSlash = filepath.find_last_of("/\\");
        lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
        auto lastDot = filepath.rfind('.');
        auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
        m_Name = filepath.substr(lastSlash, count);
    }

    OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
        : m_Name(name)
    {
        std::unordered_map<GLenum, std::string> sources;
        sources[GL_VERTEX_SHADER]   = vertexSrc;
        sources[GL_FRAGMENT_SHADER] = fragmentSrc;
        Compile(sources);
    }

    OpenGLShader::~OpenGLShader()
    {
    }

    std::string OpenGLShader::ReadFile(const std::string& filepath)
    {
        std::string result;
        std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
        if (in)
        {
            in.seekg(0, std::ios::end);
            size_t size = in.tellg();
            if (size != -1)
            {
                result.resize(size);
                in.seekg(0, std::ios::beg);
                in.read(&result[0], size);
            }
            else
            {
                KR_CORE_ERROR("读取文件 '{0}' 失败！", filepath);
            }
        }
        else
        {
            KR_CORE_ERROR("打开文件 '{0}' 失败！", filepath);
        }

        return result;
    }

    std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
    {
        std::unordered_map<GLenum, std::string> shaderSources;

        const char* typeToken = "#type";
        size_t typeTokenLength = strlen(typeToken);
        size_t pos = source.find(typeToken, 0); // 从 0 开始查找
        while (pos != std::string::npos) // npos 表示查找不到
        {
            size_t eol = source.find_first_of("\r\n", pos); // 查找换行符
            KR_CORE_ASSERT(eol != std::string::npos, "shader 格式错误！");
            size_t begin = pos + typeTokenLength + 1; // 从 typeToken 之后开始
            std::string type = source.substr(begin, eol - begin); // 截取 typeToken 之后到换行符之前的字符串
            KR_CORE_ASSERT(ShaderTypeFromString(type), "非法 shader 类型！");

            size_t nextLinePos = source.find_first_not_of("\r\n", eol); // 查找下一行
            KR_CORE_ASSERT(nextLinePos != std::string::npos, "shader 格式错误！");
            pos = source.find(typeToken, nextLinePos); // 从下一行开始查找

            // 截取下一行到下一个 typeToken 之前的字符串
            shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
        }

        return shaderSources;
    }

    void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
    {
        GLuint program = glCreateProgram();
        m_RendererID = program;

        KR_CORE_ASSERT(shaderSources.size() <= 2, "顶点和片段着色器必须成对出现！");

        int glShaderIDIndex = 0;
        std::array<GLenum, 2> glShaderIDs;

        for (auto& kv : shaderSources)
        {
            GLuint type = kv.first;
            const std::string& source = kv.second;

            GLuint shader = glCreateShader(type);

            auto sourceCStr = source.c_str();
            glShaderSource(shader, 1, &sourceCStr, 0);

            glCompileShader(shader);

            // 处理错误
            GLint isCompiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
            if (isCompiled == GL_FALSE)
            {
                GLint maxLength = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> infoLog(maxLength);
                glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

                glDeleteShader(shader);

                KR_CORE_ERROR("{0}", infoLog.data());
                KR_CORE_ASSERT(false, "Shader 编译失败！");
                break;
            }

            glAttachShader(program, shader);

            glShaderIDs[glShaderIDIndex++] = shader;

            glLinkProgram(program);

            // 处理错误
            GLint isLinked = 0;
            glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
            if (isLinked == GL_FALSE)
            {
                GLint maxLength = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> infoLog(maxLength);
                glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

                glDeleteProgram(program);

                for (auto id : glShaderIDs)
                    glDeleteShader(id);

                KR_CORE_ERROR("{0}", infoLog.data());
                KR_CORE_ASSERT(false, "Shader 链接失败！");
                return;
            }
        }

        for (auto shader : glShaderIDs)
        {
            glDetachShader(program, shader);
            glDeleteShader(shader);
        }
    }

    void OpenGLShader::Bind() const
    {
        glUseProgram(m_RendererID);
    }

    void OpenGLShader::Unbind() const
    {
        glUseProgram(0);
    }

    void OpenGLShader::SetInt(const std::string& name, int value)
    {
        glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), value);
    }

    void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
    {
        glUniform1iv(glGetUniformLocation(m_RendererID, name.c_str()), count, values);
    }

    void OpenGLShader::SetFloat(const std::string& name, float value)
    {
        glUniform1f(glGetUniformLocation(m_RendererID, name.c_str()), value);
    }

    void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
    {
        glUniform3f(glGetUniformLocation(m_RendererID, name.c_str()), value.x, value.y, value.z);
    }

    void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
    {
        glUniform4f(glGetUniformLocation(m_RendererID, name.c_str()), value.x, value.y, value.z, value.w);
    }

    void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
    {
        glUniformMatrix4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }
}