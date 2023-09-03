#include "krpch.h"
#include "LightPanel.h"

namespace Kaesar {
    static std::string LightTypeToLightName(LightType type)
    {
        if (type == LightType::Directional)
            return u8"ƽ�й�";
        if (type == LightType::Point)
            return u8"���Դ";
        if (type == LightType::Spot)
            return u8"�۹�";
        return "";
    }

    LightPanel::LightPanel()
    {
    }

    void LightPanel::DrawLight(Entity& entity)
    {
        static bool LightRemove = false;
        if (UI::DrawComponent<LightComponent>(UI::DrawIconFont(u8" �ƹ�", ICON_FA_LIGHTBULB), entity, true, &LightRemove))
        {
            auto& lightComponent = entity.GetComponent<LightComponent>();
            auto& transformComponent = entity.GetComponent<TransformComponent>();

            ImGui::Separator();
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, 80);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
            ImGui::Text(u8"�ƹ�����\0");

            ImGui::PopStyleVar();
            ImGui::NextColumn();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            std::string label = LightTypeToLightName(lightComponent.type);

            static int item_current_idx = 0;
            const char* combo_label = label.c_str();
            if (ImGui::BeginCombo(u8"##�ƹ�", combo_label))
            {
                for (int n = 0; n < 3; n++)
                {
                    const bool is_selected = (item_current_idx == n);

                    if (ImGui::Selectable(LightTypeToLightName((LightType)n).c_str(), is_selected))
                    {
                        lightComponent.type = (LightType)n;
                        if (lightComponent.type == LightType::Point)
                        {
                            lightComponent.light = std::make_shared<PointLight>();
                        }
                        if (lightComponent.type == LightType::Directional)
                        {
                            lightComponent.light = std::make_shared<DirectionalLight>();
                        }
                        if (lightComponent.type == LightType::Spot)
                        {
                            lightComponent.light = std::make_shared<SpotLight>();
                        }
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();
            ImGui::Columns(1);

            ImGui::Separator();

            auto& color4 = glm::vec4(lightComponent.light->GetColor(), 1);

            ImGui::SetNextItemWidth(60);
            ImGui::Text(u8"��ɫ\0");
            ImGui::SameLine();
            ImGuiColorEditFlags colorFlags = ImGuiColorEditFlags_HDR;
            ImGui::ColorEdit4(u8"##����ɫ", glm::value_ptr(color4), colorFlags);

            lightComponent.light->SetColor(glm::vec3(color4));

            //light's intensity
            float intensity = lightComponent.light->GetIntensity();
            ImGui::Text(u8"��   ǿ\0");
            ImGui::SameLine();
            ImGui::DragFloat("##Intensity", &intensity, 0.1, 0, 100);
            lightComponent.light->SetIntensity(intensity);

            auto PI = glm::pi<float>();

            if (lightComponent.type == LightType::Directional)
            {
                auto light = dynamic_cast<DirectionalLight*>(lightComponent.light.get());
                auto& dir = light->GetDirection();
                ImGui::SetNextItemWidth(60);
                ImGui::Text(u8"��   ��\0");
                ImGui::SameLine();
                ImGui::DragFloat3(u8"##����", glm::value_ptr(dir), 0.01f, -2 * PI, 2 * PI, "%.3f");
                light->SetDirection(dir);
            }

            if (lightComponent.type == LightType::Point)
            {
                auto light = dynamic_cast<PointLight*>(lightComponent.light.get());
                auto& position = transformComponent.Translation;
                float linear = light->GetLinear();
                float quadratic = light->GetQuadratic();
                UI::SliderFloat(u8"һ��˥��ϵ��", &linear, 0.0f, 1.0f);
                light->SetLinear(linear);
                UI::SliderFloat(u8"����˥��ϵ��", &quadratic, 0.0f, 1.0f);
                light->SetQuadratic(quadratic);
            }

            if (lightComponent.type == LightType::Spot)
            {
                auto light = dynamic_cast<SpotLight*>(lightComponent.light.get());
                auto& spotDirection = transformComponent.Rotation;
                float iCut = light->GetInnerCutOff();
                float oCut = light->GetOuterCutOff();
                float linear = light->GetLinear();
                float quadratic = light->GetQuadratic();
                UI::DragFloat(u8"�ھ�", &iCut, 0.5f, 0.0f, light->GetOuterCutOff() - 0.01f);
                UI::DragFloat(u8"�⾶", &oCut, 0.5f, light->GetOuterCutOff() + 0.01f, 180);
                UI::SliderFloat(u8"һ��ϵ��", &linear, 0.0f, 1.0f);
                UI::SliderFloat(u8"����ϵ��", &quadratic, 0.0f, 1.0f);
                light->SetCutOff(iCut, oCut);
                light->SetLinear(linear);
                light->SetQuadratic(quadratic);
            }

            ImGui::TreePop();

            ImGui::Separator();
        }

        if (LightRemove)
        {
            entity.RemoveComponent<LightComponent>();
            LightRemove = false;
        }
    }
}