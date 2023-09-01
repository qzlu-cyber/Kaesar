#pragma once

#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

namespace Kaesar {
	// 推送常量的成员
	// 推送常量是一种特殊的常量数据，可以在渲染管线中快速访问，通常用于在着色器程序中传递常量数据
	struct PCMember
	{
		std::string name; // 推送常量成员的名称
		size_t size; // 推送常量成员的大小
	};
	// 推送常量
	struct PushConstant
	{
		std::string name;
		uint32_t size;
		std::vector<PCMember> members;
	};
	// 采样器
	// 在纹理渲染中用来获取纹理数据
	struct Sampler
	{
		std::string name; // 采样器的名称，用于标识采样器在着色器代码中的位置
		uint32_t set; // 采样器所属的描述符集，用于将资源分组在一起，以供着色器使用，描述符集可以在渲染时绑定到对应的纹理或缓冲
		uint32_t binding; // 采样器在着色器中绑定的位置，用于指示具体的纹理或缓冲对象
		bool isUsed; // 采样器是否被使用
	};

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		virtual std::vector<PushConstant> GetPushConstants() = 0;
		virtual std::vector<Sampler> GetSamplers() = 0;

		virtual const std::string& GetName() const = 0;

		virtual void Reload() = 0;

		static std::shared_ptr<Shader> Create(const std::string& filepath);
		static std::shared_ptr<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	};

	class ShaderLibrary
	{
	public:
		void Add(const std::string& name, const std::shared_ptr<Shader>& shader);
		void Add(const std::shared_ptr<Shader>& shader);

		std::shared_ptr<Shader> Load(const std::string& filepath);
		std::shared_ptr<Shader> Load(const std::string& name, const std::string& filepath);

		std::shared_ptr<Shader> Get(const std::string& name);
		inline std::unordered_map<std::string, std::shared_ptr<Shader>> GetShaders() { return m_Shaders; }

		bool Exists(const std::string& name) const;

	private:
		std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
	};
}