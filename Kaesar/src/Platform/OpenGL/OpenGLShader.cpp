#include "krpch.h"
#include <fstream>
#include <glad/glad.h>
#include "Platform/OpenGL/OpenGLShader.h"
#include "glm/gtc/type_ptr.hpp"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace Kaesar {
	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		KR_CORE_ASSERT(false, "δ֪�� Shader ���ͣ�");
		return 0;
	}

	static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
	{
		switch (stage)
		{
			case GL_VERTEX_SHADER:   return shaderc_glsl_vertex_shader;
			case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
		}

		KR_CORE_ASSERT(false, "δ֪�� Shader ���ͣ�");
		return (shaderc_shader_kind)0;
	}

	static const char* GLShaderStageToString(GLenum stage)
	{
		switch (stage)
		{
			case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
			case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
		}

		KR_CORE_ASSERT(false, "δ֪�� Shader ���ͣ�");
		return nullptr;
	}

	static const char* GetCacheDirectory()
	{
		return "assets/cache/shader/opengl";
	}

	static void CreateCacheDirectoryIfNeeded()
	{
		std::string cacheDirectory = GetCacheDirectory();
		if (!std::filesystem::exists(cacheDirectory))
			std::filesystem::create_directories(cacheDirectory);
	}

	static const char* GLShaderStageCachedOpenGLFileExtension(uint32_t stage)
	{
		switch (stage)
		{
			case GL_VERTEX_SHADER:    return ".cached_opengl.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_opengl.frag";
		}

		KR_CORE_ASSERT(false, "δ֪�� Shader ���ͣ�");
		return "";
	}

	static const char* GLShaderStageCachedVulkanFileExtension(uint32_t stage)
	{
		switch (stage)
		{
			case GL_VERTEX_SHADER:    return ".cached_vulkan.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_vulkan.frag";
		}

		KR_CORE_ASSERT(false, "δ֪�� Shader ���ͣ�");
		return "";
	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
		: m_FilePath(filepath)
	{
		CreateCacheDirectoryIfNeeded();

		std::string source = ReadFile(filepath);
		auto shaderSources = PreProcess(source);

		// �õ� shader ������
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filepath.substr(lastSlash, count);

		CompileOrGetVulkanBinaries(shaderSources);
		CompileOrGetOpenGLBinaries();
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_Name(name)
	{
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;

		CompileOrGetVulkanBinaries(sources);
		CompileOrGetOpenGLBinaries();
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
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
				KR_CORE_ERROR("�ļ� '{0}' �޷��򿪣�", filepath);
			}
		}
		else
		{
			KR_CORE_ERROR("�ļ� '{0}' �޷��򿪣�'", filepath);
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0); // ��ͷ��ʼ���ң������ҵ�����ʼλ��
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos); // ���� #type ��ĵ�һ�����з��������ҵ��ĵ�һ��ƥ���ַ���λ��
			KR_CORE_ASSERT(eol != std::string::npos, "Shader �﷨����");
			size_t begin = pos + typeTokenLength + 1; // �� typeToken ֮��ʼ��� shader ����
			std::string type = source.substr(begin, eol - begin);
			KR_CORE_ASSERT(ShaderTypeFromString(type), "Kaesar ��֧�ִ�����ɫ����");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol); // ���� shader type ��ĵ�һ���ǻ��з��ַ��������ҵ��ĵ�һ��ƥ���ַ���λ��
			KR_CORE_ASSERT(nextLinePos != std::string::npos, "Shader �﷨����");
			pos = source.find(typeToken, nextLinePos); // �����Ƿ������� shader ����

			shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) 
				? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}

	/// <summary>
	/// ������ȡ Vulkan ��ɫ���� SPIR-V �ֽ��룬���ڴ˹����е��� Reflect ��������ȡ��Դ��Ϣ
	/// </summary>
	/// <param name="shaderSources">shader Դ����</param>
	void OpenGLShader::CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		const bool optimize = false;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_size);

		std::filesystem::path cacheDirectory = GetCacheDirectory();

		auto& shaderData = m_VulkanSPIRV;
		shaderData.clear();
		for (auto&& [stage, source] : shaderSources)
		{
			// ������ɫ�����ͺ��ļ�·�����������ļ�·��
			std::filesystem::path shaderFilePath = m_FilePath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + GLShaderStageCachedVulkanFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open()) // ��������ļ����ڣ��ӻ����ж�ȡ SPIR-V �ֽ���
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else // ��������ļ������ڣ�����Դ����Ϊ SPIR-V �ֽ��룬���䱣�浽�����ļ�
			{
				shaderc::SpvCompilationResult mod = compiler.CompileGlslToSpv(source, GLShaderStageToShaderC(stage), m_FilePath.c_str(), options);
				if (mod.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					KR_CORE_ERROR(mod.GetErrorMessage());
				}

				shaderData[stage] = std::vector<uint32_t>(mod.cbegin(), mod.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}

		KR_CORE_WARN("=================================={0} Shader=======================================", m_Name);
		for (auto&& [stage, data] : shaderData)
			Reflect(stage, data);
		KR_CORE_WARN("===================================================================================");
	}

	/// <summary>
	/// ���� Vulkan ��ɫ��Դ������������ SPIR-V �ֽ��룬ת��Ϊ OpenGL ��ɫ��Դ���벢����
	/// </summary>
	void OpenGLShader::CompileOrGetOpenGLBinaries()
	{
		auto& shaderData = m_OpenGLSPIRV;

		std::filesystem::path cacheDirectory = GetCacheDirectory();

		for (auto&& [stage, spirv] : m_VulkanSPIRV)
		{
			spirv_cross::CompilerGLSL glsl(std::move(m_VulkanSPIRV[stage]));
			spirv_cross::ShaderResources resources = glsl.get_shader_resources();
			spirv_cross::CompilerGLSL::Options options;

			options.version = 460;
			options.es = false;
			glsl.set_common_options(options);

			m_OpenGLSourceCode[stage] = glsl.compile();
		}

		Compile(m_OpenGLSourceCode);
	}

	/// <summary>
	/// ��ָ�����͵� OpenGL ��ɫ�����з��䣬�� SPIR-V �ֽ�������ȡ�й� Uniform ���塢�����������ͳ�������Դ����Ϣ
	/// </summary>
	/// <param name="stage">shader ����</param>
	/// <param name="shaderData">�����͵� SPIR-V �ֽ���</param>
	void OpenGLShader::Reflect(GLenum stage, const std::vector<uint32_t>& shaderData)
	{
		spirv_cross::Compiler compiler(shaderData);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();
		KR_CORE_INFO("OpenGLShader::Reflect - {0} {1}", GLShaderStageToString(stage), m_FilePath);
		KR_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
		KR_CORE_TRACE("    {0} samplers", resources.sampled_images.size());
		KR_CORE_TRACE("    {0} push constants", resources.push_constant_buffers.size());

		// ��ȡ Uniform ������Ϣ
		KR_CORE_TRACE("Uniform buffers:");
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			int memberCount = bufferType.member_types.size();

			KR_CORE_TRACE("  {0}", resource.name);
			KR_CORE_TRACE("    Size = {0}", bufferSize);
			KR_CORE_TRACE("    Binding = {0}", binding);
			KR_CORE_TRACE("    Members = {0}", memberCount);

			for (const auto& member : compiler.get_active_buffer_ranges(resource.id))
			{
				KR_CORE_TRACE("        member {0} : {1} ", member.index, compiler.get_member_name(resource.base_type_id, member.index));
			}
		}
		KR_CORE_WARN("=======================================");
		// ��ȡ��������Ϣ
		KR_CORE_TRACE("Samplers:");
		for (const auto& resource : resources.sampled_images)
		{
			unsigned set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
			unsigned binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			KR_CORE_TRACE("Image {0} at set {1}, binding = {2}", resource.name.c_str(), set, binding);
			// Modify the decoration to prepare it for GLSL.
			compiler.unset_decoration(resource.id, spv::DecorationDescriptorSet);
			m_Samplers.push_back({ resource.name, set, binding, true });
			std::sort(m_Samplers.begin(), m_Samplers.end(), [&](Sampler first, Sampler second) {
				return first.binding < second.binding;
				}
			);
		}
		KR_CORE_WARN("=======================================");
		// ��ȡ���ͳ�����Ϣ
		KR_CORE_TRACE("Push constants:");
		for (const auto& resource : resources.push_constant_buffers)
		{
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			int memberCount = bufferType.member_types.size();
			KR_CORE_TRACE("  {0}", resource.name);
			KR_CORE_TRACE("    Size = {0}", bufferSize);
			KR_CORE_TRACE("    Binding = {0}", binding);
			KR_CORE_TRACE("    Members = {0}", memberCount);
			std::vector<PCMember> members;
			for (const auto& member : compiler.get_active_buffer_ranges(resource.id))
			{
				KR_CORE_TRACE("        member {0} : {1} size = {2}"
					, member.index
					, compiler.get_member_name(resource.base_type_id, member.index)
					, compiler.get_declared_struct_member_size(bufferType, member.index)
				);
				members.push_back({ compiler.get_member_name(resource.base_type_id, member.index) , compiler.get_declared_struct_member_size(bufferType, member.index) });
			}
			m_PushConstants.push_back({ resource.name, bufferSize, members });
		}
		KR_CORE_TRACE("========================================================================================");
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();
		KR_CORE_ASSERT(shaderSources.size() < 3, "Kaesar Ŀǰ��֧�� 2 ����ɫ����");
		std::array<GLenum, 2> glShaderIDs;
		int glShaderIDIndex = 0;
		for (auto& kv : shaderSources)
		{
			GLenum type = kv.first;
			const std::string& source = kv.second;

			GLuint shader = glCreateShader(type);

			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			glCompileShader(shader);

			// ����������
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
				KR_CORE_ASSERT(false, "��ɫ������ʧ�ܣ�");
				break;
			}

			glAttachShader(program, shader);
			glShaderIDs[glShaderIDIndex++] = shader;
		}

		m_RendererID = program;

		glLinkProgram(program);

		// �������Ӵ���
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
			KR_CORE_ASSERT(false, "��ɫ������ʧ�ܣ�");
			return;
		}

		for (auto id : glShaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
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
		UploadUniformInt(name, value);
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		UploadUniformIntArray(name, values, count);
	}

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		UploadUniformFloat(name, value);
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		UploadUniformFloat3(name, value);
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		UploadUniformFloat4(name, value);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		UploadUniformMat4(name, value);
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, int value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	const std::string& OpenGLShader::GetName() const
	{
		return m_Name;
	}

	void OpenGLShader::Reload()
	{
		CreateCacheDirectoryIfNeeded();

		std::string source = ReadFile(m_FilePath);
		auto shaderSources = PreProcess(source);

		GLuint program = glCreateProgram();

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		const bool optimize = false;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_size);

		std::filesystem::path cacheDirectory = GetCacheDirectory();

		auto& shaderData = m_VulkanSPIRV;
		shaderData.clear();
		for (auto&& [stage, source] : shaderSources)
		{
			std::filesystem::path shaderFilePath = m_FilePath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + GLShaderStageCachedVulkanFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);

			shaderc::SpvCompilationResult mod = compiler.CompileGlslToSpv(source, GLShaderStageToShaderC(stage), m_FilePath.c_str(), options);
			if (mod.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				KR_CORE_ERROR(mod.GetErrorMessage());
			}

			shaderData[stage] = std::vector<uint32_t>(mod.cbegin(), mod.cend());

			std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
			if (out.is_open())
			{
				auto& data = shaderData[stage];
				out.write((char*)data.data(), data.size() * sizeof(uint32_t));
				out.flush();
				out.close();
			}

		}
		KR_CORE_WARN("=================================={0} Shader=======================================", m_Name);
		for (auto&& [stage, data] : shaderData)
			Reflect(stage, data);
		KR_CORE_WARN("===================================================================================");

		CompileOrGetOpenGLBinaries();
	}
}