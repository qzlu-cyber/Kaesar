#pragma once

#include "../UI.h"

#include "Kaesar/Scene/Entity.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Kaesar
{
	class MeshPanel
	{
	public:
		MeshPanel();
		~MeshPanel() = default;

		void DrawMesh(Entity& entity);
	};
}