#include "hzpch.h"
#include "WindowsInput.h"
#include "Hazel/Application.h"

#include <GLFW/glfw3.h>

namespace Hazel {
    Input* Input::s_Instance = new WindowsInput(); // 这里是将 WindowsInput 的实例赋值给 Input 的 s_Instance，所以在 WindowsInput 中实现的虚函数，就会被调用。

    bool Hazel::WindowsInput::IsKeyPressedImpl(int keycode)
    {
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetKey(window, keycode); // 两个参数，(GLFWwindow* window, int key) 在哪个窗口按下了哪个键，所以要获取到窗口。WindowsWindow 中有一个 private 的 GLFWwindow* m_Window; 的成员变量，所以要获取到这个成员变量。

        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool WindowsInput::IsMouseButtonPressedImpl(int button)
    {
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetMouseButton(window, button);

        return state == GLFW_PRESS;
    }

    std::pair<float, float> WindowsInput::GetMousePositionImpl()
    {
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        return std::pair<float, float>(static_cast<float>(xpos), static_cast<float>(ypos));
    }

    float WindowsInput::GetMouseXImpl()
    {
        auto [x, y] = GetMousePositionImpl();

        return x;
    }

    float WindowsInput::GetMouseYImpl()
    {
        auto [x, y] = GetMousePositionImpl();

        return y;
    }
}