#pragma once

#include "../UI.h"

#include "Kaesar/Scene/Entity.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Kaesar {
    class CameraPanel
    {
    public:
        CameraPanel();
        ~CameraPanel() = default;

        void DrawCamera(Entity& entity);
    };
}
