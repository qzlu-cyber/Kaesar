#pragma once

#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

namespace Kaesar {
	// ���ͳ����ĳ�Ա
	// ���ͳ�����һ������ĳ������ݣ���������Ⱦ�����п��ٷ��ʣ�ͨ����������ɫ�������д��ݳ�������
	struct PCMember
	{
		std::string name; // ���ͳ�����Ա������
		size_t size; // ���ͳ�����Ա�Ĵ�С
	};
	// ���ͳ���
	struct PushConstant
	{
		std::string name;
		uint32_t size;
		std::vector<PCMember> members;
	};
	// ������
	// ��������Ⱦ��������ȡ��������
	struct Sampler
	{
		std::string name; // �����������ƣ����ڱ�ʶ����������ɫ�������е�λ��
		uint32_t set; // �������������������������ڽ���Դ������һ���Թ���ɫ��ʹ�ã�����������������Ⱦʱ�󶨵���Ӧ������򻺳�
		uint32_t binding; // ����������ɫ���а󶨵�λ�ã�����ָʾ���������򻺳����
		bool isUsed; // �������Ƿ�ʹ��
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