#include "krpch.h"
#include "WindowsInput.h"
#include "Kaesar/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Kaesar {
    Input* Input::s_Instance = new WindowsInput(); // �����ǽ� WindowsInput ��ʵ����ֵ�� Input �� s_Instance�������� WindowsInput ��ʵ�ֵ��麯�����ͻᱻ���á�

    bool Kaesar::WindowsInput::IsKeyPressedImpl(int keycode)
    {
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetKey(window, keycode); // ����������(GLFWwindow* window, int key) ���ĸ����ڰ������ĸ���������Ҫ��ȡ�����ڡ�WindowsWindow ����һ�� private �� GLFWwindow* m_Window; �ĳ�Ա����������Ҫ��ȡ�������Ա������

        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool WindowsInput::IsMouseButtonPressedImpl(int button)
    {
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetMouseButton(window, button);

        return state == GLFW_PRESS;
    }

    bool WindowsInput::IsMouseButtonReleasedImpl(int button)
    {
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetMouseButton(window, button);

        return state == GLFW_RELEASE;
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