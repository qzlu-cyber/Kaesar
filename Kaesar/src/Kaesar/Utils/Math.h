#pragma once

#include <glm/glm.hpp>

namespace Kaesar::Math {
    // �� 4x4 �任����ֽ�Ϊƽ�ơ���ת���������
    bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation,
                            glm::vec3& rotation, glm::vec3& scale);
}