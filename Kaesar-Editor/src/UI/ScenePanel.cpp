#include "krpch.h"
#include "ScenePanel.h"

#include <imgui/imgui.h>

namespace Kaesar {
    ScenePanel::ScenePanel(const std::shared_ptr<Scene>& scene)
    {
        m_Context = scene;
    }

    void ScenePanel::OnImGuiRender()
    {
        ImGui::ShowStyleEditor(); // �� ImGui ��ʽ�༭�����Զ��� ImGui ��������
        ImGui::Begin(u8"����");

        m_Context->m_Registry.each([&](auto entityID)
            {
                Entity entity = { entityID, m_Context.get() };
                DrawEntity(entity);
            });

        ImGui::End();
    }

    void ScenePanel::DrawEntity(Entity entity)
    {
        auto& tag = entity.GetComponent<TagComponent>();

        // ȷ��������Ⱦ��ʾʵ������ڵ�ı�־�����ʵ�嵱ǰ�Ƿ�ѡ�У��Լ��ڵ��Ƿ�Ӧ�ڵ�����ͷʱչ��
        ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

        // �ڽ����г���һ�����ڵ㣬ʹ��ָ���ı�־�ͱ�ǩ�ı����˺������õĽ��ָʾ�ڵ��Ǵ򿪻��ǹر�
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.Tag.c_str());
        if (ImGui::IsItemClicked()) { // �Ƿ����˵�ǰ�����ڵ㣨ʵ�壩
            m_SelectionContext = entity; // ѡ�е�ǰ��ʵ��
        }

        if (opened) { // �����ǰ�����ڵ㣨ʵ�壩�Ǵ򿪵�
            ImGui::TreePop(); // �رյ�ǰ�����ڵ㣨ʵ�壩
        }
    }
}