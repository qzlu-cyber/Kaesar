#pragma once

#include "Kaesar/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Kaesar {
    class OpenGLContext : public GraphicsContext
    {
    public:
        OpenGLContext(GLFWwindow* windowHandle);

    public:
        virtual void Init() override;
        virtual void SwapBuffers() override;

    private:
        GLFWwindow* m_WindowHandle;
    };
}
