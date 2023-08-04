#include "krpch.h"

#include "PerspectiveCamera.h"

#include "Kaesar/Core/KeyCodes.h"
#include "Kaesar/Core/MouseButtonCodes.h"
#include "Kaesar/Core/Input.h"

#include <glad/glad.h>
#include <glm/gtx/quaternion.hpp>

namespace Kaesar {
    PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip)
        : m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip),
        Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
    {
        UpdateView();
    }

    void PerspectiveCamera::UpdateProjection()
    {
        m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
        m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
    }

    void PerspectiveCamera::UpdateView()
    {
        m_Position = CalculatePosition(); // 首先得到摄像机的位置

        glm::quat orientation = GetOrientation(); // 然后得到摄像机的方向
        // 将摄像机位置始终视作原点，即和世界坐标中的原点重合，再让观察空间的坐标轴与世界空间坐标轴重合即可得到观察矩阵
        // 由于摄像机在由世界空间中的变换是先旋转再平移，故 ViewMatrix = R * T (即先做平移变换 T，再做旋转变换)
        // 首先将摄像机的位置平移到原点，即做平移变换 T
        // 然后将摄像机的方向与世界坐标轴重合，即做旋转变换 R，直接计算矩阵 R 比较难算，可逆向来看，将世界坐标轴旋转到摄像机的方向再取逆即可得到结果
        // glm 矩阵以列主序存储，即先存储第一列，再存储第二列，以此类推。故 orientation 即为将世界坐标轴旋转到摄像机的方向
        // 因 glm::toMat4(orientation) 为正交矩阵，故其逆又等于其转置，所以它就是 R
        // m_ViewMatrix = glm::toMat4(orientation) * glm::translate(glm::mat4(1.0f), -m_Position);
        // 上面更好理解，但是先平移后旋转，这样会导致旋转中心不在原点，而是在摄像机的位置，这样会导致旋转的轨迹不是以摄像机为中心的球体，而是以摄像机为中心的椭球体
        // 对上述过程取逆，即可得到正确的结果
        m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
        m_ViewMatrix = glm::inverse(m_ViewMatrix);
    }

    std::pair<float, float> PerspectiveCamera::PanSpeed() const
    {
        float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
        float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

        float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
        float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

        return { xFactor, yFactor };
    }

    float PerspectiveCamera::RotationSpeed() const
    {
        return 0.8f;
    }

    float PerspectiveCamera::ZoomSpeed() const
    {
        float distance = m_Distance * 0.2f;
        distance = std::max(distance, 0.0f);
        float speed = distance * distance;
        speed = std::min(speed, 100.0f); // max speed = 100
        return speed;
    }

    void PerspectiveCamera::OnUpdate(const Timestep& timestep)
    {
        if (Input::IsKeyPressed(KR_KEY_LEFT_ALT))
        {
            const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
            glm::vec2 delta = (mouse - m_InitialMousePosition) * 2.0f; // delta 为鼠标移动的距离
            m_InitialMousePosition = mouse;

            if (Input::IsMouseButtonPressed(KR_MOUSE_BUTTON_MIDDLE))
                MousePan(delta * timestep.GetSeconds());
            else if (Input::IsMouseButtonPressed(KR_MOUSE_BUTTON_LEFT))
                MouseRotate(delta * timestep.GetSeconds());
            else if (Input::IsMouseButtonPressed(KR_MOUSE_BUTTON_RIGHT))
                MouseZoom(delta.y * timestep.GetSeconds());
        }

        if (Input::IsMouseButtonPressed(KR_MOUSE_BUTTON_RIGHT))
        {
            // 使用 timestep 来调整平移的速度
            float panSpeedX = PanSpeed().first * 20.0f * timestep.GetSeconds();
            float panSpeedY = PanSpeed().second * 20.0f * timestep.GetSeconds();

            if (Input::IsKeyPressed(KR_KEY_A))
                m_FocalPoint -= GetRightDirection() * panSpeedX;
            else if (Input::IsKeyPressed(KR_KEY_D))
                m_FocalPoint += GetRightDirection() * panSpeedX;
            else if (Input::IsKeyPressed(KR_KEY_W))
                m_FocalPoint += GetForwardDirection() * panSpeedY;
            else if (Input::IsKeyPressed(KR_KEY_S))
                m_FocalPoint -= GetForwardDirection() * panSpeedY;
        }

        UpdateView();
    }

    void PerspectiveCamera::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseScrolledEvent>(KR_BIND_EVENT_FN(PerspectiveCamera::OnMouseScroll));
        dispatcher.Dispatch<WindowResizeEvent>([&](WindowResizeEvent e) { return OnResize(e); });
    }

    bool PerspectiveCamera::OnMouseScroll(MouseScrolledEvent& e)
    {
        float delta = e.GetYOffset() * 0.1f;
        MouseZoom(delta);
        UpdateView();
        return false;
    }

    bool PerspectiveCamera::OnResize(WindowResizeEvent& e) {
        m_ViewportWidth  = static_cast<float>(e.GetWidth());
        m_ViewportHeight = static_cast<float>(e.GetHeight());

        UpdateProjection();

        glViewport(0, 0, m_ViewportWidth, m_ViewportHeight);

        return false;
    }

    void PerspectiveCamera::MousePan(const glm::vec2& delta)
    {
        auto [xSpeed, ySpeed] = PanSpeed();
        m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
        m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
    }

    void PerspectiveCamera::MouseRotate(const glm::vec2& delta)
    {
        float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
        m_Yaw += yawSign * delta.x * RotationSpeed();
        m_Pitch += delta.y * RotationSpeed();
    }

    void PerspectiveCamera::MouseZoom(float delta)
    {
        m_Distance -= delta * ZoomSpeed(); // 将摄像机与焦点间的距离缩放
        if (m_Distance < 1.0f) // 防止摄像机进入焦点内部
        {
            if (m_FocalPoint.z > 0.0) { // 焦点在摄像机前方
                m_FocalPoint += GetForwardDirection(); // 将焦点向摄像机后方移动
            }
            m_Distance = 1.0f; // 将摄像机的距离限制在最小值 1.0
        }
    }

    glm::vec3 PerspectiveCamera::GetUpDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::vec3 PerspectiveCamera::GetRightDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    glm::vec3 PerspectiveCamera::GetForwardDirection() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
    }

    glm::vec3 PerspectiveCamera::CalculatePosition() const
    {
        return m_FocalPoint - GetForwardDirection() * m_Distance;
    }

    glm::quat PerspectiveCamera::GetOrientation() const
    {
        return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
    }

    void PerspectiveCamera::SetYawPitch(float yaw, float pitch)
    {
        m_Yaw = yaw;
        m_Pitch = pitch;
        UpdateView();
    }
}