#include "krpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Kaesar {
    OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
        : m_WindowHandle(windowHandle) 
    {
        KR_CORE_ASSERT(windowHandle, "Window handle is null!")
    }

    void OpenGLContext::Init() {
        glfwMakeContextCurrent(m_WindowHandle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress); // gladLoadGLLoader ����ݲ�ͬ��ƽ̨�����ò�ͬ�ĺ���ȥ���� OpenGL ����ָ��
        KR_CORE_ASSERT(status, "Could not intialize Glad!");
    }

    void OpenGLContext::SwapBuffers() {
        glfwSwapBuffers(m_WindowHandle);
    }
}