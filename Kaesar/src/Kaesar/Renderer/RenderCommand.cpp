#include "krpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Kaesar {
    RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}