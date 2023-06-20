#pragma once

#include "hzpch.h"
#include "Hazel/Core.h"

/// <summary>
/// �� Application �����Լ��Ĵ��� Window����Ӧ�� Window ��Ӧ��֪���κ� Application ����Ϣ��
/// ���Ի�Ҫͬʱ����һ�� callback�������� Window �� Application ��������ʱ����
/// </summary>

namespace Hazel {
    // Events in Hazel are currently blocking, meaning when an event occurs it
    // immediately gets dispatched and must be dealt with right then an there.
    // For the future, a better strategy might be to buffer events in an event
    // bus and process them during the "event" part of the update stage.

    // �¼�������
    enum class EventType
    {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
        AppTick, AppUpdate, AppRender,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };

    // �¼��ķ���
    enum EventCategory
    {
        None = 0,
        EventCategoryApplication = BIT(0),
        EventCategoryInput = BIT(1),
        EventCategoryKeyboard = BIT(2),
        EventCategoryMouse = BIT(3),
        EventCategoryMouseButton = BIT(4)
    };

    // ʹ�ú�����ظ�����
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
                               virtual EventType GetEventType() const override { return GetStaticType(); }\
							   virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

    // �¼��Ļ���
    class HAZEL_API Event
    {
        friend class EventDispatcher;
    public:
        // �ж��¼��Ƿ񱻴���
        bool m_Handled = false;

        // �����¼�������
        virtual EventType GetEventType() const = 0;
        // �����¼��ķ���
        virtual int GetCategoryFlags() const = 0;
        // �����¼�������
        virtual const char* GetName() const = 0;
        // �����¼����ַ�����ʾ
        virtual std::string ToString() const { return GetName(); }

        // �ж��¼��Ƿ�����ĳ������
        /// һ���¼��������Ƕ�� Category�����������������¼������� EventCategoryMouseButton��Ҳ��EventCategoryMouse��
        /// ͬʱ���� EventCategoryInput����ĳһ�¼����������ڵ� Category ������ true
        inline bool IsInCategory(EventCategory category)
        {
            return GetCategoryFlags() & category;
        }
    };

    // �¼��ɷ�������һ���¼���һ����������һ�𣬵��������¼�������ʱ���ͼ����Ӧ�ĺ���
    class EventDispatcher
    {
        template<typename T>
        using EventFn = std::function<bool(T&)>; // ����ָ�룬����һ�� T ���͵����ã����� bool
    public:
        EventDispatcher(Event& event)
            : m_Event(event)
        {
        }

        // �ɷ��¼�
        template<typename T>
        bool Dispatch(EventFn<T> func)
        {
            if (m_Event.GetEventType() == T::GetStaticType())
            {
                m_Event.m_Handled = func(*(static_cast<T*>(&m_Event))); // �� m_Event ȡ��ַ��ת��һ�� T ��ָ�룬Ȼ�������
                return true;
            }
            return false;
        }
    private:
        Event& m_Event;

        // ����¼����ַ�����ʾ
        friend std::ostream& operator<<(std::ostream& os, const Event& e)
        {
            return os << e.ToString();
        }
    };
}