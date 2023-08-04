#include "krpch.h"
#include "Entity.h"

namespace Kaesar {
	Entity::Entity(entt::entity handle, Scene* scene)
		:m_EntityID(handle), m_Scene(scene)
	{}
}
