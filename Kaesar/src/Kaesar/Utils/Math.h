#pragma once

#include <glm/glm.hpp>

namespace Kaesar::Math {
    // 将 4x4 变换矩阵分解为平移、旋转和缩放组件
    bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation,
                            glm::vec3& rotation, glm::vec3& scale);
}