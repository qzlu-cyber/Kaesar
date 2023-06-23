#include "hzpch.h"
#include "OrthographicCamera.h"

#include "Hazel/KeyCodes.h"
#include "Hazel/MouseButtonCodes.h"
#include "Platform/Windows/WindowsInput.h"

namespace Hazel {
    float OrthographicCamera::fov = 90.0f;

    OrthographicCamera::OrthographicCamera(const glm::vec3& position, float yaw, float pitch, float lastX, float lastY, bool firstMouse)
        : m_CameraPosition(position), yaw(yaw), pitch(pitch), lastX(lastX), lastY(lastY),firstMouse(firstMouse)
    {
        m_CameraDirection = glm::normalize(m_CameraPosition);
        m_CameraRight     = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_CameraDirection));
        m_CameraUp        = glm::cross(m_CameraPosition, m_CameraRight);
        m_ViewMatrix      = glm::mat4(1.0f);
    }

    void OrthographicCamera::CameraFreeMove(GLFWwindow* window, float deltaTime)
    {
        float cameraSpeed = 0.01f * deltaTime;

        if (WindowsInput::IsKeyPressed(HZ_KEY_W))
        {
            m_CameraPosition += cameraSpeed * (-m_CameraDirection);
        }
        if (WindowsInput::IsKeyPressed(HZ_KEY_S))
        {
            m_CameraPosition -= cameraSpeed * (-m_CameraDirection);
        }
        if (WindowsInput::IsKeyPressed(HZ_KEY_A))
        {
            m_CameraPosition -= glm::normalize(glm::cross((-m_CameraDirection), m_CameraUp)) * cameraSpeed;
        }
        if (WindowsInput::IsKeyPressed(HZ_KEY_D))
        {
            m_CameraPosition += glm::normalize(glm::cross((-m_CameraDirection), m_CameraUp)) * cameraSpeed;
        }

        if (WindowsInput::IsMouseButtonPressed(HZ_MOUSE_BUTTON_RIGHT))
        {
            auto [xPos, yPos] = WindowsInput::GetMousePosition();
            OrthographicCamera::CameraMovement(xPos, yPos);
        }

        glfwSetScrollCallback(window, OrthographicCamera::CameraScale);
    }

    void OrthographicCamera::CameraMovement(float xPos, float yPos)
    {
        if (firstMouse)
        {
            lastX = xPos;
            lastY = yPos;
            firstMouse = false;
        }

        float xOffset = xPos - lastX;
        float yOffset = lastY - yPos; // reversed since y-coordinates go from bottom to top
        lastX = xPos;
        lastY = yPos;

        float sensitivity = 0.1f; // change this value to your liking
        xOffset *= sensitivity;
        yOffset *= sensitivity;

        yaw += xOffset;
        pitch += yOffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front = glm::vec3(1.0f);
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        m_CameraDirection = -glm::normalize(front);
    }

    void OrthographicCamera::CameraScale(GLFWwindow* window, double xOffset, double yOffset)
    {
        fov -= static_cast<float>(yOffset);
        if (fov >= 1.0f && fov <= 45.0f)
            fov -= yOffset;
        if (fov < 1.0f)
            fov = 1.0f;
    }
}