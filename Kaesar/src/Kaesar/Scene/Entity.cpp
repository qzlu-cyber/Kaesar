#include "krpch.h"
#include "Entity.h"

namespace Kaesar {
	Scene* Entity::s_Scene = nullptr;

	Entity::Entity(entt::entity handle)
		: m_EntityID(handle)
	{}
}
