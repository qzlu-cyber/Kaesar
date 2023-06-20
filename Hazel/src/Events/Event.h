#pragma once

#include "hzpch.h"
#include "Hazel/Core.h"

/// <summary>
/// 由 Application 创建自己的窗口 Window，对应的 Window 不应该知道任何 Application 的信息，
/// 所以还要同时创建一个 callback，用来在 Window 和 Application 传递数据时调用
/// </summary>

namespace Hazel {
    // Events in Hazel are currently blocking, meaning when an event occurs it
    // immediately gets dispatched and must be dealt with right then an there.
    // For the future, a better strategy might be to buffer events in an event
    // bus and process them during the "event" part of the update stage.

    // 事件的类型
    enum class EventType
    {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
        AppTick, AppUpdate, AppRender,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };

    // 事件的分类
    enum EventCategory
    {
        None = 0,
        EventCategoryApplication = BIT(0),
        EventCategoryInput = BIT(1),
        EventCategoryKeyboard = BIT(2),
        EventCategoryMouse = BIT(3),
        EventCategoryMouseButton = BIT(4)
    };

    // 使用宏代替重复代码
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
                               virtual EventType GetEventType() const override { return GetStaticType(); }\
							   virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

    // 事件的基类
    class HAZEL_API Event
    {
        friend class EventDispatcher;
    public:
        // 判断事件是否被处理
        bool m_Handled = false;

        // 返回事件的类型
        virtual EventType GetEventType() const = 0;
        // 返回事件的分类
        virtual int GetCategoryFlags() const = 0;
        // 返回事件的名称
        virtual const char* GetName() const = 0;
        // 返回事件的字符串表示
        virtual std::string ToString() const { return GetName(); }

        // 判断事件是否属于某个分类
        /// 一个事件，可以是多个 Category，如鼠标左键点击这个事件，既是 EventCategoryMouseButton，也是EventCategoryMouse，
        /// 同时还是 EventCategoryInput。对某一事件，是它属于的 Category 都返回 true
        inline bool IsInCategory(EventCategory category)
        {
            return GetCategoryFlags() & category;
        }
    };

    // 事件派发器。将一个事件和一个函数绑定在一起，当监听的事件被触发时，就激活对应的函数
    class EventDispatcher
    {
        template<typename T>
        using EventFn = std::function<bool(T&)>; // 函数指针，接受一个 T 类型的引用，返回 bool
    public:
        EventDispatcher(Event& event)
            : m_Event(event)
        {
        }

        // 派发事件
        template<typename T>
        bool Dispatch(EventFn<T> func)
        {
            if (m_Event.GetEventType() == T::GetStaticType())
            {
                m_Event.m_Handled = func(*(static_cast<T*>(&m_Event))); // 对 m_Event 取地址，转成一个 T 的指针，然后解引用
                return true;
            }
            return false;
        }
    private:
        Event& m_Event;

        // 输出事件的字符串表示
        friend std::ostream& operator<<(std::ostream& os, const Event& e)
        {
            return os << e.ToString();
        }
    };
}