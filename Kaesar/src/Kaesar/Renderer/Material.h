#pragma once

#include "Kaesar/Renderer/Shader.h"

#include <memory>

namespace Kaesar {
    class Material
    {
    public:

        Material(std::shared_ptr<Shader>& shader);

        static std::shared_ptr<Material> Create(std::shared_ptr<Shader>& shader);
    };
}