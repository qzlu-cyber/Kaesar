#include "krpch.h"
#include "CameraPanel.h"

namespace Kaesar {
    CameraPanel::CameraPanel()
    {
    }

    void CameraPanel::DrawCamera(Entity& entity)
    {
        static bool CameraRemove = false;
        if (UI::DrawComponent<CameraComponent>(u8"���", entity, true, &CameraRemove))
        {
            CameraComponent& cameraComponent = entity.GetComponent<CameraComponent>();
            SceneCamera& camera = cameraComponent.Camera;

            ImGui::Checkbox(u8"�����", &cameraComponent.Primary);

            const char* projectionType[] = { "Perspective", "Orthographic" };
            const char* currentProjectionType = projectionType[(int)camera.GetProjectionType()];
            // �����˵�������ѡ�������ͶӰ����
            if (ImGui::BeginCombo(u8"ͶӰ��ʽ", currentProjectionType)) // ��ȡ��ǰ�����ͶӰ���Ͳ�����Ϊ�����˵��ĵ�ǰѡ����
            {
                // ʹ��ѭ����������ѡ�������ѡ��ĳ����ʱ���������ͶӰ����
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
                if (UI::DragFloat(u8"��Ұ", &perspectiveFov, 0.5f, 10.0f, 189.0f))
                    camera.SetPerspectiveFOV(glm::radians(perspectiveFov));

                float perspectiveNear = camera.GetPerspectiveNearClip();
                if (UI::DragFloat(u8"��ƽ��", &perspectiveNear, 0.1f, 0.001f, 100.0f))
                    camera.SetPerspectiveNearClip(perspectiveNear);

                float perspectiveFar = camera.GetPerspectiveFarClip();
                if (UI::DragFloat(u8"Զƽ��", &perspectiveFar, 0.1f, 100.0f, 10000.0f))
                    camera.SetPerspectiveFarClip(perspectiveFar);
            }

            if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
            {
                float orthoSize = camera.GetOrthographicSize();
                if (UI::DragFloat(u8"�ߴ�", &orthoSize, 0.5f, 0.01f, 100.0f))
                    camera.SetOrthographicSize(orthoSize);

                float orthoNear = camera.GetOrthographicNearClip();
                if (UI::DragFloat(u8"��ƽ��", &orthoNear, 0.5f, 0.01f, 100.0f))
                    camera.SetOrthographicNearClip(orthoNear);

                float orthoFar = camera.GetOrthographicFarClip();
                if (UI::DragFloat(u8"Զƽ��", &orthoFar, 0.5f, 0.01f, 100.0f))
                    camera.SetOrthographicFarClip(orthoFar);

                ImGui::Checkbox(u8"�̶���߱�", &cameraComponent.FixedAspectRatio);
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