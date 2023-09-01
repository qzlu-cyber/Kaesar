#pragma once

#include "../UI.h"

#include "Kaesar/Scene/Entity.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Kaesar {

	class MaterialPanel
	{
	public:
		MaterialPanel();
		~MaterialPanel() = default;

		void DrawMaterial(Entity& entity);

	private:
		std::shared_ptr<Texture2D> m_EmptyTexture;
		ImTextureID m_TextureId;
	};
}