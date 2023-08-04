#pragma once

/// Window 抽象类，Application 中创建使用的 Window 类
/// 不同平台的 Window 不同，抽象一层接口后在不同平台分别单独实现

#include "krpch.h"
#include "Core.h"

#include "Events/Event.h"

namespace Kaesar {
    struct WindowProps {
        std::string Title; // 窗口名
        uint32_t Width; // 窗口宽度
        uint32_t Height; // 窗口高度

        // 初始化窗口
        WindowProps(const std::string& title = "Kaesar Rendering Engine",
            uint32_t width = 1920,
            uint32_t height = 1080)
            : Title(title), Width(width), Height(height)
        {
        }
    };

    class Kaesar_API Window {
    public:
        using EventCallbackFn = std::function<void(Event&)>; // 事件回调函数。返回值为 void，参数为 Event&

        virtual ~Window() {};

        virtual void OnUpdate() = 0;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;

        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        virtual void* GetNativeWindow() const = 0; // 为了在外部使用 GLFWwindow* m_Window; 的成员变量，所以要获取到这个成员变量。

        // 在每个平台上被实现
        static Window* Create(const WindowProps& props = WindowProps());
    };
}