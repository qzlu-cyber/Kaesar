#pragma once

#include "Kaesar/Core/Timestep.h"
#include "Kaesar/Renderer/Camera/PerspectiveCamera.h"

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

		void DestroyEntity(Entity entity);

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, std::shared_ptr<PerspectiveCamera>& camera);
		void OnViewportResize(uint32_t width, uint32_t height);


	private:
		entt::registry m_Registry; // container of all entities and components

		uint32_t m_ViewportWidth = 0;
		uint32_t m_ViewportHeight = 0;

		friend class Entity;
	};
}
