#include "krpch.h"
#include "CameraPanel.h"

namespace Kaesar {
    CameraPanel::CameraPanel()
    {
    }

    void CameraPanel::DrawCamera(Entity& entity)
    {
        static bool CameraRemove = false;
        if (UI::DrawComponent<CameraComponent>(u8"相机", entity, true, &CameraRemove))
        {
            CameraComponent& cameraComponent = entity.GetComponent<CameraComponent>();
            SceneCamera& camera = cameraComponent.Camera;

            ImGui::Checkbox(u8"主相机", &cameraComponent.Primary);

            const char* projectionType[] = { "Perspective", "Orthographic" };
            const char* currentProjectionType = projectionType[(int)camera.GetProjectionType()];
            // 下拉菜单，用于选择相机的投影类型
            if (ImGui::BeginCombo(u8"投影方式", currentProjectionType)) // 获取当前相机的投影类型并设置为下拉菜单的当前选择项
            {
                // 使用循环绘制两个选择项，并在选择某个项时更新相机的投影类型
                for (int i = 0; i < 2; i++)
                {
                    bool isSelected = currentProjectionType == projectionType[i];
                    if (ImGui::Selectable(projectionType[i], isSelected))
                    {
                        currentProjectionType = projectionType[i];
                        camera.SetProjectionType((SceneCamera::ProjectionType)i);
                    }

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }

                ImGui::EndCombo();
            }

            if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
            {
                float perspectiveFov = glm::degrees(camera.GetPerspectiveFOV());
                if (UI::DragFloat(u8"视野", &perspectiveFov, 0.5f, 10.0f, 189.0f))
                    camera.SetPerspectiveFOV(glm::radians(perspectiveFov));

                float perspectiveNear = camera.GetPerspectiveNearClip();
                if (UI::DragFloat(u8"近平面", &perspectiveNear, 0.1f, 0.001f, 100.0f))
                    camera.SetPerspectiveNearClip(perspectiveNear);

                float perspectiveFar = camera.GetPerspectiveFarClip();
                if (UI::DragFloat(u8"远平面", &perspectiveFar, 0.1f, 100.0f, 10000.0f))
                    camera.SetPerspectiveFarClip(perspectiveFar);
            }

            if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
            {
                float orthoSize = camera.GetOrthographicSize();
                if (UI::DragFloat(u8"尺寸", &orthoSize, 0.5f, 0.01f, 100.0f))
                    camera.SetOrthographicSize(orthoSize);

                float orthoNear = camera.GetOrthographicNearClip();
                if (UI::DragFloat(u8"近平面", &orthoNear, 0.5f, 0.01f, 100.0f))
                    camera.SetOrthographicNearClip(orthoNear);

                float orthoFar = camera.GetOrthographicFarClip();
                if (UI::DragFloat(u8"远平面", &orthoFar, 0.5f, 0.01f, 100.0f))
                    camera.SetOrthographicFarClip(orthoFar);

                ImGui::Checkbox(u8"固定宽高比", &cameraComponent.FixedAspectRatio);
            }

            ImGui::TreePop();

            ImGui::Separator();
        }

        if (CameraRemove)
        {
            entity.RemoveComponent<CameraComponent>();
            CameraRemove = false;
        }
    }
}