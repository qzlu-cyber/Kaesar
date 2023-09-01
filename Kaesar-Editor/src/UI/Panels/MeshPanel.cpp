#include "krpch.h"
#include "MeshPanel.h"

#include "Kaesar/Utils/PlatformUtils.h"

namespace Kaesar {
    MeshPanel::MeshPanel()
    {
    }

    void MeshPanel::DrawMesh(Entity& entity)
    {
        static bool MeshRemove = false;
        if (UI::DrawComponent<MeshComponent>(u8"模型", entity, true, &MeshRemove))
        {
            auto& tag = entity.GetComponent<MeshComponent>().path;

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy_s(buffer, tag.c_str());

            if (ImGui::InputText(u8"路径", buffer, sizeof(buffer)))
            {
                tag = std::string(buffer);
            }

            ImGui::SameLine();

            if (ImGui::Button(u8"选择"))
            {
                auto dir = std::filesystem::current_path();
                std::optional<std::string> path = Kaesar::FileDialogs::OpenFile("模型文件 (*.obj, *.fbx)\0*.obj;*.fbx\0");
                if (path)
                {
                    std::string filepath;
                    if (path->find(dir.string()) != std::string::npos)
                    {
                        filepath = path->substr(dir.string().size());
                    }
                    else
                    {
                        filepath = *path;
                    }
                    tag = filepath;
                    entity.GetComponent<MeshComponent>().model = Model(filepath);
                }
            }

            ImGui::TreePop();

            ImGui::Separator();
        }

        if (MeshRemove)
        {
            entity.RemoveComponent<MeshComponent>();
            MeshRemove = false;
        }
    }
}