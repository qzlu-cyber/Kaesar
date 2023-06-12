#pragma once

#include <GLFW/glfw3.h>

#include "Hazel/Window.h"

namespace Hazel {

    class HAZEL_API WindowsWindow : public Window {
    public:
        WindowsWindow(const WindowProps& props);
        virtual ~WindowsWindow();

        void OnUpdate() override;

        inline unsigned int GetWidth() const override { return m_Data.Width; }
        inline unsigned int GetHeight() const override { return m_Data.Height; }

        inline virtual void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
        void SetVSync(bool enabled) override;
        bool IsVSync() const override;

        inline virtual void* GetNativeWindow() const { return m_Window; } // 为了在外部使用 GLFWwindow* m_Window; 的成员变量，所以要获取到这个成员变量。

    private:
        virtual void Init(const WindowProps& props);
        virtual void Shutdown();

    private:
        GLFWwindow* m_Window;

        // 将数据传递给 GLFW
        struct WindowData
        {
            std::string Title;
            unsigned int Width, Height;
            bool VSync;

            EventCallbackFn EventCallback;
        };

        WindowData m_Data;
    };
}