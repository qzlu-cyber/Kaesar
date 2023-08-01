#include "krpch.h"

#include "PerspectiveCamera.h"

#include "Kaesar/Core/KeyCodes.h"
#include "Kaesar/Core/MouseButtonCodes.h"
#include "Kaesar/Core/Input.h"

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
        // m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation
        m_Position = CalculatePosition(); // ���ȵõ��������λ��

        glm::quat orientation = GetOrientation(); // Ȼ��õ�������ķ���
        // ���������λ��Ӧ�õ���λ�����ϡ����ƽ�Ʊ任�Ľ������һ����������ʾ�����������������ϵ�е�λ��
        // ��������ķ���ת��Ϊһ����ת����
        // ��������������ˣ��õ��Ľ�����ǹ۲��������۲����������������λ�ú���ת��Ϣ
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
            glm::vec2 delta = (mouse - m_InitialMousePosition) * 2.0f; // delta Ϊ����ƶ��ľ���
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
            // ʹ�� timestep ������ƽ�Ƶ��ٶ�
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
        dispatcher.Dispatch<WindowResizeEvent>([&](WindowResizeEvent e) {
            return OnResize(e);
            });
    }

    bool PerspectiveCamera::OnMouseScroll(MouseScrolledEvent& e)
    {
        float delta = e.GetYOffset() * 0.1f;
        MouseZoom(delta);
        UpdateView();
        return false;
    }

    bool PerspectiveCamera::OnResize(WindowResizeEvent& e) {
        m_ViewportWidth = e.GetWidth();
        m_ViewportHeight = e.GetHeight();
        //UpdateProjection();
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
        m_Distance -= delta * ZoomSpeed(); // ��������뽹���ľ�������
        if (m_Distance < 1.0f) // ��ֹ��������뽹���ڲ�
        {
            if (m_FocalPoint.z > 0.0) { // �����������ǰ��
                m_FocalPoint += GetForwardDirection(); // ����������������ƶ�
            }
            m_Distance = 1.0f; // ��������ľ�����������Сֵ 1.0
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