#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Kaesar/Core/Timestep.h"

namespace Kaesar {
    class Kaesar_API Layer {
    public:
        Layer(const std::string& name = "Layer");
        virtual ~Layer();

        virtual void OnAttach() {} // 当被推到 LayerStack 上时调用
        virtual void OnDetach() {} // 当被移除时调用。这两个类似于窗口的 Init() 和 OnShundown()
        virtual void OnImGuiRender() {} // 当层被推到 ImGui 层上时调用
        virtual void OnUpdate(const Timestep& timestep) {} // 当层更新时，每帧都要调用
        virtual void OnEvent(Event& event) {} // 每当一个事件位于中间层时，在此处被接收

        inline const std::string& GetName() const { return m_DebugName; }
    protected:
        std::string m_DebugName;

        bool isEnabled = true; // 标记该层是否被禁用。很显然，被禁用的层无法执行上述的方法
    };
}
