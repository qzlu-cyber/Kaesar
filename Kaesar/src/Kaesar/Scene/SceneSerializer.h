#pragma once

#include "Kaesar/Scene/Scene.h"

namespace Kaesar {
    class SceneSerializer
    {
    public:
        SceneSerializer(const std::shared_ptr<Scene>& scene);

        void Serializer(const std::string& filepath);
        bool Deserializer(const std::string& filepath);

    private:
        std::shared_ptr<Scene> m_Scene;
        ShaderLibrary m_Shaders;
    };
}
