#pragma once

#include "../UI.h"

#include "Kaesar/Scene/Entity.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Kaesar {
	class LightPanel
	{
	public:
		LightPanel();
		~LightPanel() = default;

		void DrawLight(Entity& entity);
	};
}