#pragma once

/// Window 抽象类，Application 中创建使用的 Window 类
/// 不同平台的 Window 不同，抽象一层接口后在不同平台分别单独实现

#include "hzpch.h"
#include "Hazel/Core.h"
#include "Events/Event.h"

namespace Hazel {

    struct WindowProps {
        std::string Title; // 窗口名
        unsigned int Width; // 窗口宽度
        unsigned int Height; // 窗口高度

        // 初始化窗口
        WindowProps(const std::string& title = "Hazel Engine",
            unsigned int width = 1280,
            unsigned int height = 720)
            : Title(title), Width(width), Height(height)
        {
        }
    };

    class HAZEL_API Window {
    public:
        using EventCallbackFn = std::function<void(Event&)>; // 事件回调函数。返回值为 void，参数为 Event&

        virtual ~Window() {};

        virtual void OnUpdate() = 0;

        virtual unsigned int GetWidth() const = 0;
        virtual unsigned int GetHeight() const = 0;

        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        virtual void* GetNativeWindow() const = 0; // 为了在外部使用 GLFWwindow* m_Window; 的成员变量，所以要获取到这个成员变量。

        // 在每个平台上被实现
        static Window* Create(const WindowProps& props = WindowProps());
    };
}