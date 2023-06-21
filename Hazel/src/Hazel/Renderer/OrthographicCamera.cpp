#include "hzpch.h"
#include "OrthographicCamera.h"

#include "Hazel/KeyCodes.h"
#include "Hazel/MouseButtonCodes.h"
#include "Platform/Windows/WindowsInput.h"
#include "Hazel/Application.h"

#include <GLFW/glfw3.h>

namespace Hazel {
    bool OrthographicCamera::firstMouse = true;
    float OrthographicCamera::yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
    float OrthographicCamera::pitch = 0.0f;
    float OrthographicCamera::lastX = 1280.0f / 2.0;
    float OrthographicCamera::lastY = 720.0 / 2.0;
    float OrthographicCamera::fov = 90.0f;

    glm::vec3 OrthographicCamera::cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

    OrthographicCamera::OrthographicCamera()
    {
        m_CameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
        m_CameraDirection = glm::normalize(m_CameraPosition - glm::vec3(0.0f, 0.0f, 0.0f));
        m_CameraRight = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_CameraDirection));
        m_CameraUp = glm::cross(m_CameraDirection, m_CameraRight);

        m_ViewMatrix = glm::lookAt(m_CameraPosition, m_CameraPosition + cameraFront, cameraUp);
    }

    void OrthographicCamera::SetViewMatrix(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up)
    {
        m_ViewMatrix = glm::lookAt(eye, center, up);
    }

    void OrthographicCamera::CameraFreeMove()
    {
        float deltaTime = 0.0f; // 当前帧与上一帧的时间差
        float lastFrame = 0.0f; // 上一帧的时间
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

        float cameraSpeed = 0.01f * deltaTime;

        if (WindowsInput::IsKeyPressed(HZ_KEY_W))
        {
            m_CameraPosition += cameraSpeed * cameraFront;
        }
        if (WindowsInput::IsKeyPressed(HZ_KEY_S))
        {
            m_CameraPosition -= cameraSpeed * cameraFront;
        }
        if (WindowsInput::IsKeyPressed(HZ_KEY_A))
        {
            m_CameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        if (WindowsInput::IsKeyPressed(HZ_KEY_D))
        {
            m_CameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }

        if (WindowsInput::IsMouseButtonPressed(HZ_MOUSE_BUTTON_RIGHT))
        {
            auto [xPos, yPos] = WindowsInput::GetMousePosition();
            OrthographicCamera::CameraMovement(xPos, yPos);
        }

        glfwSetScrollCallback(window, OrthographicCamera::CameraScale);

        OrthographicCamera::SetViewMatrix(m_CameraPosition, m_CameraPosition + cameraFront, cameraUp);
    }

    void OrthographicCamera::CameraMovement(float xPos, float yPos)
    {
        if (firstMouse)
        {
            lastX = xPos;
            lastY = yPos;
            firstMouse = false;
        }

        float xoffset = xPos - lastX;
        float yoffset = lastY - yPos; // reversed since y-coordinates go from bottom to top
        lastX = xPos;
        lastY = yPos;

        float sensitivity = 0.1f; // change this value to your liking
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front = glm::vec3(1.0f);
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
    }

    void OrthographicCamera::CameraScale(GLFWwindow* window, double xOffset, double yOffset)
    {
        fov -= static_cast<float>(yOffset);
        if (fov < 1.0f)
            fov = 1.0f;
        if (fov > 45.0f)
            fov = 45.0f;
    }
}