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
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());
		Entity DuplicateEntity(Entity entity);
		entt::entity FindEntity(uint32_t id);

		void DestroyEntity(Entity entity);

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, PerspectiveCamera& camera);
		void OnViewportResize(uint32_t width, uint32_t height);

		uint32_t GetMainTextureID() { return SceneRenderer::GetTextureID(0); }
		std::shared_ptr<FrameBuffer> GetMouseFB() { return SceneRenderer::GetMouseFB(); }
		FramebufferSpecification GetSpec() { return SceneRenderer::GetMainFBSpec(); }

	private:
		entt::registry m_Registry; // container of all entities and components

		std::vector<std::shared_ptr<Entity>> m_Entities; // container of all entities

		uint32_t m_ViewportWidth = 0;
		uint32_t m_ViewportHeight = 0;

		friend class Entity;
		friend class ScenePanel;
		friend class SceneSerializer;
		friend class SceneRenderer;
	};
}
