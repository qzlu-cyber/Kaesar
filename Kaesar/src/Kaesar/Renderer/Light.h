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
		Light(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float intensity)
            : m_Ambient(ambient), m_Diffuse(diffuse), m_Specular(specular), m_Intensity(intensity)
		{}

		virtual ~Light() = default;

		void SetAmbient(const glm::vec3& ambient) { m_Ambient = ambient; }
		glm::vec3 GetAmbient() const { return m_Ambient; }

		void SetDiffuse(const glm::vec3& diffuse) { m_Diffuse = diffuse; }
		glm::vec3 GetDiffuse() const { return m_Diffuse; }

		void SetSpecular(const glm::vec3& specular) { m_Specular = specular; }
		glm::vec3 GetSpecular() const { return m_Specular; }

		void SetIntensity(float intensity) { m_Intensity = intensity; }
		float GetIntensity() { return m_Intensity; }

	private:
		glm::vec3 m_Ambient;
		glm::vec3 m_Diffuse;
		glm::vec3 m_Specular;

		float m_Intensity = 1;
	};


	class DirectionalLight : public Light 
	{
	public:
		DirectionalLight()
			: Light(glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.5f, 0.5f, 0.5f), 1.0f)
		{}
		DirectionalLight(const glm::vec3& dir, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float intensity)
            : Light(ambient, diffuse, specular, intensity), m_Direction(dir)
        {}

		virtual ~DirectionalLight() = default;

		void SetDirection(const glm::vec3& dir) { m_Direction = dir; }
		glm::vec3 GetDirection() const { return m_Direction; }

	private:
		glm::vec3 m_Direction = { 1.0f, 1.0f, 1.0f };
	};


	class PointLight : public Light 
	{
	public:
		PointLight() 
			: Light(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f)
		{}
		PointLight(const glm::vec3& pos, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular,
				   float intensity, float linear, float quadratic)
            : Light(ambient, diffuse, specular, intensity), m_Position(pos), m_Linear(linear), m_Quadratic(quadratic)
        {}

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
		SpotLight()
			: Light(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f)
		{}
		SpotLight(const glm::vec3& pos, const glm::vec3& dir, 
				  const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float intensity,
				  float linear, float quadratic, float cutOff, float outerCutOff)
            : Light(ambient, diffuse, specular, intensity), m_Position(pos), m_Direction(dir), 
			  m_Linear(linear), m_Quadratic(quadratic), m_CutOff(cutOff), m_OuterCutOff(outerCutOff)
        {}

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