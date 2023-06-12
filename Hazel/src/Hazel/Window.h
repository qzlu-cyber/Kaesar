#pragma once

/// Window �����࣬Application �д���ʹ�õ� Window ��
/// ��ͬƽ̨�� Window ��ͬ������һ��ӿں��ڲ�ͬƽ̨�ֱ𵥶�ʵ��

#include "hzpch.h"
#include "Hazel/Core.h"
#include "Events/Event.h"

namespace Hazel {

    struct WindowProps {
        std::string Title; // ������
        unsigned int Width; // ���ڿ��
        unsigned int Height; // ���ڸ߶�

        // ��ʼ������
        WindowProps(const std::string& title = "Hazel Engine",
            unsigned int width = 1280,
            unsigned int height = 720)
            : Title(title), Width(width), Height(height)
        {
        }
    };

    class HAZEL_API Window {
    public:
        using EventCallbackFn = std::function<void(Event&)>; // �¼��ص�����������ֵΪ void������Ϊ Event&

        virtual ~Window() {};

        virtual void OnUpdate() = 0;

        virtual unsigned int GetWidth() const = 0;
        virtual unsigned int GetHeight() const = 0;

        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        virtual void* GetNativeWindow() const = 0; // Ϊ�����ⲿʹ�� GLFWwindow* m_Window; �ĳ�Ա����������Ҫ��ȡ�������Ա������

        // ��ÿ��ƽ̨�ϱ�ʵ��
        static Window* Create(const WindowProps& props = WindowProps());
    };
}