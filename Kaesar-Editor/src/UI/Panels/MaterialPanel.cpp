#include "krpch.h"
#include "MaterialPanel.h"

#include "Kaesar/Utils/PlatformUtils.h"

namespace Kaesar {
    MaterialPanel::MaterialPanel()
    {
        m_EmptyTexture = Texture2D::Create("assets/models/cube/default.png", 0);
        m_TextureId = reinterpret_cast<void*>(m_EmptyTexture->GetRendererID());
    }

    void MaterialPanel::DrawMaterial(Entity& entity)
    {
        static bool MaterialRemove = false;
        if (UI::DrawComponent<MaterialComponent>(u8"材质", entity, true, &MaterialRemove))
        {
            auto& materialComponent = entity.GetComponent<MaterialComponent>();

            ImGui::Separator();
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, 80);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
            ImGui::Text("Shader\0");

            ImGui::PopStyleVar();
            ImGui::NextColumn();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

            ImGui::Text("PBR shader\0");
            ImGui::PopItemWidth();
            ImGui::Columns(1);
            ImGui::Separator();

            std::vector<Sampler>& samplers = materialComponent.material.GetSamplers();
            auto& materialTextures = materialComponent.material.GetTextures();

            const auto& buffer = materialComponent.material.GetCBuffer();
            float tiling = buffer.tiling;
            if (UI::DragFloat("Tiling", &tiling, 0.05f, 0.001f, 100))
            {
                materialComponent.material.Set("tiling", tiling);
            }

            for (auto& sampler : samplers)
            {
                ImGui::PushID(sampler.name.c_str());
                ImGui::Separator();
                int frame_padding = -1 + 0;                           // -1 == uses default padding (style.FramePadding)
                ImVec2 size = ImVec2(64.0f, 64.0f);                  // Size of the image we want to make visible
                ImGui::Checkbox(u8"启用", &sampler.isUsed);
                ImGui::SameLine();
                ImGui::Text(sampler.name.c_str());

                m_TextureId = reinterpret_cast<void*>(m_EmptyTexture->GetRendererID());
                auto& texture = materialComponent.material.GetTexture(sampler);
                if (texture)
                {
                    m_TextureId = reinterpret_cast<void*>(texture->GetRendererID());
                }

                if (ImGui::ImageButton(m_TextureId, size, ImVec2{ 0, 1 }, ImVec2{ 1, 0 }))
                {
                    auto path = FileDialogs::OpenFile("Kaesar Texture (*.*)\0*.*\0");
                    if (path)
                    {
                        // 内嵌纹理默认为漫反射贴图，绑定点为 0 就是漫反射贴图，将其设置为 sRGB 颜色空间
                        materialTextures[sampler.binding] = Texture2D::Create(*path, 0, sampler.binding == 0);
                    }
                }

                // Albedo
                if (sampler.binding == 0)
                {
                    glm::vec4 color = buffer.material.color;
                    if (ImGui::ColorEdit4(u8"反照率", glm::value_ptr(color), ImGuiColorEditFlags_NoInputs))
                    {
                        materialComponent.material.Set("pc.material.color", color);
                    }
                }
                // Metallic
                if (sampler.binding == 1)
                {
                    float metallic = buffer.material.MetallicFactor;

                    if (UI::SliderFloat(u8"金属度", &metallic, 0.0f, 1.0f))
                    {
                        materialComponent.material.Set("pc.material.MetallicFactor", metallic);
                    }
                }
                // Roughness
                if (sampler.binding == 3)
                {
                    float roughness = buffer.material.RoughnessFactor;

                    if (UI::SliderFloat(u8"粗糙度", &roughness, 0.0f, 1.0f))
                    {
                        materialComponent.material.Set("pc.material.RoughnessFactor", roughness);
                    }
                }
                // AO
                if (sampler.binding == 4)
                {
                    float AO = buffer.material.AO;

                    if (UI::SliderFloat(u8"环境光遮蔽", &AO, 0.0f, 1.0f))
                    {
                        materialComponent.material.Set("pc.material.AO", AO);
                    }
                }

                ImGui::PopID();
            }
            ImGui::TreePop();
            ImGui::Separator();
        }

        if (MaterialRemove)
        {
            entity.RemoveComponent<MaterialComponent>();
            MaterialRemove = false;
        }
    }
}