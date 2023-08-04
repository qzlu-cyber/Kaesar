#pragma once

/// Window �����࣬Application �д���ʹ�õ� Window ��
/// ��ͬƽ̨�� Window ��ͬ������һ��ӿں��ڲ�ͬƽ̨�ֱ𵥶�ʵ��

#include "krpch.h"
#include "Core.h"

#include "Events/Event.h"

namespace Kaesar {
    struct WindowProps {
        std::string Title; // ������
        uint32_t Width; // ���ڿ��
        uint32_t Height; // ���ڸ߶�

        // ��ʼ������
        WindowProps(const std::string& title = "Kaesar Rendering Engine",
            uint32_t width = 1920,
            uint32_t height = 1080)
            : Title(title), Width(width), Height(height)
        {
        }
    };

    class Kaesar_API Window {
    public:
        using EventCallbackFn = std::function<void(Event&)>; // �¼��ص�����������ֵΪ void������Ϊ Event&

        virtual ~Window() {};

        virtual void OnUpdate() = 0;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;

        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        virtual void* GetNativeWindow() const = 0; // Ϊ�����ⲿʹ�� GLFWwindow* m_Window; �ĳ�Ա����������Ҫ��ȡ�������Ա������

        // ��ÿ��ƽ̨�ϱ�ʵ��
        static Window* Create(const WindowProps& props = WindowProps());
    };
}