#pragma once

#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Events/ApplicationEvent.h"

#include "Camera.h"

namespace Kaesar {
    class PerspectiveCamera : public Camera
    {
    public:
        PerspectiveCamera() = default;
        PerspectiveCamera(float fov, float aspectRatio, float near, float far);

        void OnUpdate();
        void OnEvent(Event& e);
        
        inline float GetDistance() const { return m_Distance; }
        inline void SetDistance(float distance) { m_Distance = distance; }
        glm::vec3 GetFocalPoint() { return m_FocalPoint; }
        void SetFocalPoint(glm::vec3 position) { m_FocalPoint = position; UpdateView(); }
        inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }

        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }

        glm::vec3 GetUpDirection() const; // 摄像机上方向
        glm::vec3 GetRightDirection() const; // 摄像机右方向
        glm::vec3 GetForwardDirection() const; // 摄像机前方向
        const glm::vec3& GetPosition() const { return m_Position; }
        glm::quat GetOrientation() const; // 获取摄像机方向

        float GetPitch() const { return m_Pitch; }
        float GetYaw() const { return m_Yaw; }

        void SetYawPitch(float yaw, float pitch);

        float GetFOV() { return m_FOV; }
        void SetFov(float fov) { m_FOV = fov; UpdateProjection(); }

        float GetNear() { return m_NearClip; }
        void SetNearClip(float nearClip) { m_NearClip = nearClip; UpdateProjection(); }

        float GetFar() { return m_FarClip; }
        void SetFarClip(float farClip) { m_FarClip = farClip; UpdateProjection(); }

        ~PerspectiveCamera() = default;

    private:
        void UpdateProjection();
        void UpdateView();

        bool OnMouseScroll(MouseScrolledEvent& e);
        bool OnResize(WindowResizeEvent& e);

        void MousePan(const glm::vec2& delta);
        void MouseRotate(const glm::vec2& delta);
        void MouseZoom(float delta);

        glm::vec3 CalculatePosition() const;

        std::pair<float, float> PanSpeed() const;
        float RotationSpeed() const;
        float ZoomSpeed() const;


    private:
        float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.01f, m_FarClip = 1000.0f;
        float m_ViewportWidth = 1280.0f, m_ViewportHeight = 720.0f;

        glm::mat4 m_ViewMatrix;
        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f }; // 摄像机位置
        glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f }; // 摄像机焦点

        float m_Distance = 10.0f; // 摄像机与焦点距离
        glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f }; // 鼠标初始位置

        float m_Pitch = 0.0f; // 俯仰角
        float m_Yaw = 0.0f; // 偏航角
    };
}

