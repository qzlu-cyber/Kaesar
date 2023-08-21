#pragma once

#include "Kaesar/Core/Timestep.h"
#include "Kaesar/Renderer/Camera/PerspectiveCamera.h"
#include "Kaesar/Renderer/FrameBuffer.h"
#include "Kaesar/Renderer/SceneRenderer.h"

#include <entt.hpp>
#include <memory>

namespace Kaesar {
	class Entity;

	class Scene
	{
	public:
		Scene(const std::string& name = u8"未命名");
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());
		std::shared_ptr<Entity> Scene::CreateEntity(Entity other);
		entt::entity FindEntity(uint32_t id);

		void DestroyEntity(Entity entity);

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts);
		void OnViewportResize(uint32_t width, uint32_t height);

		uint32_t GetMainTextureID() { return SceneRenderer::GetTextureID(0); }
		std::shared_ptr<FrameBuffer> GetMainFrameBuffer() { return SceneRenderer::GetGeoFrameBuffer(); }
		FramebufferSpecification GetSpec() { return SceneRenderer::GetMainFrameSpec(); }

		ShaderLibrary& GetShaderLibrary() { return m_Shaders; }
		void SetShaderLibrary(const ShaderLibrary& shaders) { m_Shaders = shaders; }

	private:
		entt::registry m_Registry; // container of all entities and components

		std::vector<std::shared_ptr<Entity>> m_Entities; // container of all entities

		ShaderLibrary m_Shaders; // 所有的 shader

		PerspectiveCamera* m_Camera; // 主摄像机

		std::string m_Name; // 场景名称

		uint32_t m_ViewportWidth = 0;
		uint32_t m_ViewportHeight = 0;

		friend class Entity;
		friend class ScenePanel;
		friend class SceneSerializer;
		friend class SceneRenderer;
		friend class EditorLayer;
	};
}
