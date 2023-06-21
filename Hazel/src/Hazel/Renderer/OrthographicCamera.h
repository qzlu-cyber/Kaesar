#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "D:\CPP\Hazel\Hazel\vendor\GLFW\include\GLFW\glfw3.h"

namespace Hazel {
    class OrthographicCamera
    {
    public:
        OrthographicCamera();

        const glm::vec3& GetPosition() const { return m_CameraPosition; }
        void SetPosition(const glm::vec3& position) { m_CameraPosition = position; }

        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        void SetViewMatrix(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);

        const float& GetFOV() const { return fov; }

        void CameraFreeMove(); // 上下左右平移相机
        void CameraMovement(float xPos, float yPos); // 转动相机
        static void CameraScale(GLFWwindow* window, double xOffset, double yOffset); // 缩放相机

    private:
        glm::vec3 m_CameraPosition; // 相机位置
        glm::vec3 m_CameraDirection; // 摄像机正 z 轴方向
        glm::vec3 m_CameraRight;  // 摄像机 x 轴的正方向
        glm::vec3 m_CameraUp; // 摄像机 y 轴的正方向
        glm::mat4 m_ViewMatrix; // view 变换矩阵

        static glm::vec3 cameraFront;
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

        static bool  firstMouse;
        static float yaw; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
        static float pitch;
        static float lastX;
        static float lastY;
        static float fov;
    };
}