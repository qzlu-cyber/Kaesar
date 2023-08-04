#include "krpch.h"
#include "ScenePanel.h"

#include <cstring>
#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Kaesar {
    ScenePanel::ScenePanel(const std::shared_ptr<Scene>& scene)
    {
        m_Context = scene;
    }

    void ScenePanel::OnImGuiRender()
    {
        ImGui::ShowStyleEditor(); // �� ImGui ��ʽ�༭�����Զ��� ImGui ��������

        /// ====================== scene ========================
        ImGui::Begin(u8"����");

        for (auto entity : m_Context->m_Entities)
        {
            DrawEntity(*entity);
        }

        // �������������˵�ǰ�Ĵ��ڣ����ҵ�ǰ�Ĵ�������ͣ��
        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
            m_SelectionContext = {}; // ���ѡ�е�ʵ��

        // �������Ҽ�����˵�ǰ�Ĵ��ڣ�����һ������ʽ�����Ĳ˵�
        if (ImGui::BeginPopupContextWindow(0, 1))
        {
            if (ImGui::Selectable(u8"�½�����")) {
                m_Context->CreateEntity();
            }
            ImGui::EndPopup();
        }

        ImGui::End();

        /// ===================== Properties =======================
        ImGui::Begin(u8"����");

        if (m_SelectionContext)
        {
            DrawComponents(m_SelectionContext);
            ImGui::SetNextItemWidth(-1);
            // TODO: Add Component
            if (ImGui::Button(u8"������")) 
            {

            }
        }

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

        // ʵ����ק����
        for (int n = 0; n < m_Context->m_Entities.size(); n++)
        {
            auto item = m_Context->m_Entities[n];
            // ����Ƿ���һ�� ImGui ��Ŀ���϶����Ҳ�����ͣ��
            if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
            {
                int n_next = n + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1); // ���������ק�ķ���������һ��λ�õ�����
                if (n_next >= 0 && n_next < m_Context->m_Entities.size()) // �߽��飬��������ʵ���λ��
                {
                    m_Context->m_Entities[n] = m_Context->m_Entities[n_next];
                    m_Context->m_Entities[n_next] = item;
                    ImGui::ResetMouseDragDelta(); // ���������ק�ı仯��
                }
            }
        }

        if (opened) { // �����ǰ�����ڵ㣨ʵ�壩�Ǵ򿪵�
            ImGui::TreePop(); // �رյ�ǰ�����ڵ㣨ʵ�壩
        }
    }

    void ScenePanel::DrawComponents(Entity entity)
    {
        if (entity.HasComponent<TagComponent>()) {

            if (ImGui::TreeNodeEx((void*)typeid(TagComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, u8"����")) {

                auto& tag = entity.GetComponent<TagComponent>().Tag;

                char buffer[256];
                memset(buffer, 0, sizeof(buffer));
                strcpy_s(buffer, tag.c_str());

                // ����һ���ı�����������û��༭һ��ʵ��� tag
                if (ImGui::InputText("", buffer, sizeof(buffer))) {
                    tag = std::string(buffer); // ����ʵ��� tag
                }

                if (ImGui::IsItemClicked()) {
                    m_SelectionContext = entity; // ѡ�е�ǰ��ʵ��
                }
                ImGui::TreePop();
            }
        }

        if (entity.HasComponent<TransformComponent>()) {

            if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, u8"�任")) {
                auto& translate = entity.GetComponent<TransformComponent>().Translation;
                auto& scale = entity.GetComponent<TransformComponent>().Scale;
                auto& rot = entity.GetComponent<TransformComponent>().Rotation;


                ImGui::TextColored(ImVec4(.8f, .1f, .5f, 1.0f), u8"λ��");
                ImGui::DragFloat3("", glm::value_ptr(translate), 0.1f);

                ImGui::TextColored(ImVec4(.8f, .1f, .5f, 1.0f), u8"��ת");
                ImGui::DragFloat3("1", glm::value_ptr(rot), 0.5f, 0, 180);

                ImGui::TextColored(ImVec4(.8f, .1f, .5f, 1.0f), u8"����");
                ImGui::DragFloat3("2", glm::value_ptr(scale), 0.1f);

                ImGui::TreePop();
            }

        }
    }
}