#pragma once

#include <glm/glm.hpp>

namespace Kaesar {
	enum class LightType
	{
		Directional = 0,
		Point,
		Spot
	};

	class Light 
	{
	public:
		Light() = default;
		Light(const glm::vec3& color, float intensity) 
			: m_Color(color), m_Intensity(intensity) 
		{}

		virtual ~Light() = default;

		void SetColor(const glm::vec3& color) { m_Color = color; }
		glm::vec3 GetColor() const { return m_Color; }

		void SetIntensity(float intensity) { m_Intensity = intensity; }
		float GetIntensity() const { return m_Intensity; }

	private:
		glm::vec3 m_Color = { 1.0f, 1.0f, 1.0f };

		float m_Intensity = 1.0f;
	};


	class DirectionalLight : public Light 
	{
	public:
		DirectionalLight() = default;
		DirectionalLight(const glm::vec3& color) : Light(color, 1.0f) 
		{}
		DirectionalLight(const glm::vec3& color, float intensity) : Light(color, intensity) {}
		DirectionalLight(const glm::vec3& dir, const glm::vec3& color, float intensity)
			: Light(color, intensity), m_Direction(dir) {}
		DirectionalLight(const DirectionalLight& light) 
		{
			this->SetColor(light.GetColor());
			this->SetIntensity(light.GetIntensity());
		}

		virtual ~DirectionalLight() = default;

		void SetDirection(const glm::vec3& dir) { m_Direction = dir; }
		glm::vec3 GetDirection() const { return m_Direction; }

	private:
		glm::vec3 m_Direction = { -1.0f, 1.0f, 0.0f };
	};


	class PointLight : public Light 
	{
	public:
		PointLight() = default;
		PointLight(const glm::vec3& color) : Light(color, 1.0f) {}
		PointLight(const glm::vec3& color, float intensity) : Light(color, intensity) {}
		PointLight(const glm::vec3& pos, const glm::vec3& color)
			: Light(color, 1.0f), m_Position(pos) {}
		PointLight(const glm::vec3& pos, const glm::vec3& color, float intensity, float linear, float quadratic)
			: Light(color, intensity), m_Position(pos), m_Linear(linear), m_Quadratic(quadratic) {}

		PointLight(const PointLight& light) 
		{
			this->SetColor(light.GetColor());
			this->SetIntensity(light.GetIntensity());
		}

		virtual ~PointLight() = default;

		void SetPosition(const glm::vec3& pos) { m_Position = pos; }
		glm::vec3 GetPosition() const { return m_Position; }

		void SetLinear(float linear) { m_Linear = linear; }
		float GetLinear() const { return m_Linear; }

		void SetQuadratic(float quadratic) { m_Quadratic = quadratic; }
		float GetQuadratic() const { return m_Quadratic; }

	private:
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		float m_Linear = 0.09f;
		float m_Quadratic = 0.032f;
	};


	class SpotLight : public Light 
{
	public:
		SpotLight() = default;
		SpotLight(const glm::vec3& color) : Light(color, 1.0f) {}
		SpotLight(const glm::vec3& color, float intensity) : Light(color, intensity) {}
		SpotLight(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& color, float intensity, float linear, float quadratic, float cutOff, float outerCutOff)
			: Light(color, intensity), m_Position(pos), m_Direction(dir), m_Linear(linear), m_Quadratic(quadratic), m_CutOff(cutOff), m_OuterCutOff(outerCutOff) {}

		SpotLight(const SpotLight& light)
		{
			this->SetColor(light.GetColor());
			this->SetIntensity(light.GetIntensity());
		}

		virtual ~SpotLight() = default;

		void SetPosition(const glm::vec3& pos) { m_Position = pos; }
		glm::vec3 GetPosition() const { return m_Position; }

		void SetDirection(const glm::vec3& dir) { m_Direction = dir; }
		glm::vec3 GetDirection() const { return m_Direction; }

		void SetLinear(float linear) { m_Linear = linear; }
		float GetLinear() const { return m_Linear; }

		void SetQuadratic(float quadratic) { m_Quadratic = quadratic; }
		float GetQuadratic() const { return m_Quadratic; }

		void SetCutOff(float cutOff, float outerCutOff) { m_CutOff = cutOff; m_OuterCutOff = outerCutOff; }
		float GetInnerCutOff() const { return m_CutOff; }
		float GetOuterCutOff() const { return m_OuterCutOff; }

	private:
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Direction = { -1.0f, 0.0f, 0.0f };

		float m_Linear = 0.09f;
		float m_Quadratic = 0.032f;

		float m_CutOff = 12.5f;
		float m_OuterCutOff = 15.0f;
	};
}