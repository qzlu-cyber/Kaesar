#pragma once

#include "Kaesar/Core/Layer.h"
#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"

namespace Kaesar {
    class Kaesar_API ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnImGuiRender() override;
        virtual void OnEvent(Event& event) override;

        void Begin();
        void End();

        inline void SetBlockEvents(bool block) { m_BlockEvents = block; }

        void SetDarkThemeColors();

    private:
        float m_Time = 0.0f;
        bool m_BlockEvents = true; // 标记 ImGui 是否被阻塞
    };
}
