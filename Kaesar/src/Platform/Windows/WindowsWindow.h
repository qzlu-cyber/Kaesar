#pragma once

#include "Kaesar/Core/Window.h"
#include "Kaesar/Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>

namespace Kaesar {
    class Kaesar_API WindowsWindow : public Window {
    public:
        WindowsWindow(const WindowProps& props);
        virtual ~WindowsWindow();

        void OnUpdate() override;

        inline uint32_t GetWidth() const override { return m_Data.Width; }
        inline uint32_t GetHeight() const override { return m_Data.Height; }

        inline virtual void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
        void SetVSync(bool enabled) override;
        bool IsVSync() const override;

        inline virtual void* GetNativeWindow() const { return m_Window; } // Ϊ�����ⲿʹ�� GLFWwindow* m_Window; �ĳ�Ա����������Ҫ��ȡ�������Ա������

        virtual void SetWindowTitle(const std::string& title) override;

    private:
        virtual void Init(const WindowProps& props);
        virtual void Shutdown();

    private:
        GraphicsContext* m_Context;
        GLFWwindow* m_Window;

        // �����ݴ��ݸ� GLFW
        struct WindowData
        {
            std::string Title;
            uint32_t Width, Height;
            bool VSync;

            EventCallbackFn EventCallback;
        };

        WindowData m_Data;
    };
}