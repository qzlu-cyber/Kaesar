#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "D:\CPP\Kaesar\Kaesar\vendor\GLFW\include\GLFW\glfw3.h"

namespace Kaesar {
    class OrthographicCamera
    {
    public:
        OrthographicCamera(
            const glm::vec3& position = glm::vec3(0.0f, 0.0f, 3.0f), 
            float yaw = -90.0f, float pitch = 0.0f, 
            float lastX = static_cast<float>(1280 / 2), float lastY = static_cast<float>(720 / 2),
            bool firstMouse = true
        );

        const glm::vec3& GetPosition() const { return m_CameraPosition; }
        void SetPosition(const glm::vec3& position) { m_CameraPosition = position; }

        const glm::mat4& GetViewMatrix() const { return glm::lookAt(m_CameraPosition, m_CameraPosition + (-m_CameraDirection), m_CameraUp); }

        const float& GetFOV() const { return fov; }

        void CameraFreeMove(GLFWwindow* window, float deltaTime); // 上下左右平移相机
        void CameraMovement(float xPos, float yPos); // 转动相机
        static void CameraScale(GLFWwindow* window, double xOffset, double yOffset); // 缩放相机

    private:
        glm::vec3 m_CameraPosition; // 相机位置
        glm::vec3 m_CameraDirection; // 摄像机正 z 轴方向
        glm::vec3 m_CameraRight;  // 摄像机 x 轴的正方向
        glm::vec3 m_CameraUp; // 摄像机 y 轴的正方向
        glm::mat4 m_ViewMatrix; // view 变换矩阵

        bool  firstMouse;
        float yaw; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
        float pitch;
        float lastX;
        float lastY;
        static float fov;
    };
}