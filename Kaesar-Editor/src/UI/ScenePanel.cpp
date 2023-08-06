#include "krpch.h"
#include "ScenePanel.h"

#include <cstring>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
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

        for (auto& entity : m_Context->m_Entities)
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
            if (ImGui::Button(u8"�������")) 
            {

            }
        }

        ImGui::End();
    }

    void ScenePanel::DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
    {
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];

        ImGui::PushID(label.c_str());

        // �����в����л��������ؼ�
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth); // ���õ�һ�еĿ��
        // ���Ʊ�ǩ
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 5 }); // ���ÿؼ�֮��ļ��
        ImGui::Text(label.c_str());

        // ���� checkbox
        if (label == u8"����") 
        {
            ImGui::SameLine();
            // ������ѡ�������ߵļ��
            float marginLeft = 10.0f;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + marginLeft);
            // ���浱ǰ�Ĺ��λ��
            float startPosX = ImGui::GetCursorPosX();
            // ���õ�ѡ��Ŀ��
            float checkboxWidth = 20.0f;
            ImGui::SetNextItemWidth(checkboxWidth);
            // ����һ���Զ���ĵ�ѡ��
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0)); // ȥ���ڱ߾�
            ImGui::Checkbox("##L", &m_ScaleLock);
            ImGui::PopStyleVar();
            // �ָ����λ��
            ImGui::SetCursorPosX(startPosX + checkboxWidth);
            // �ָ��������
            ImGui::SetNextItemWidth(-1); // ���ÿ������
        }

        ImGui::PopStyleVar();
        ImGui::NextColumn(); // �л�����һ��

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f; // �����и�
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight }; // ���ð�ť�Ĵ�С

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth()); // �������������ؼ��Ŀ��
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 5 }); // ���ÿؼ�֮��ļ��

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("X", buttonSize))
            values.x = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
        if (label == u8"����" && m_ScaleLock)
        {
            values.y = values.x;
            values.z = values.x;
        }
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushFont(boldFont);
        if (ImGui::Button("Y", buttonSize))
            values.y = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
        if (label == u8"����" && m_ScaleLock)
        {
            values.x = values.y;
            values.z = values.y;
        }
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushFont(boldFont);

        if (ImGui::Button("Z", buttonSize))
            values.z = resetValue;
        ImGui::PopFont();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
        if (label == u8"����" && m_ScaleLock)
        {
            values.x = values.z;
            values.y = values.z;
        }
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);

        ImGui::PopID();
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
            auto&item = m_Context->m_Entities[n];
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

                ImGui::PushID("TagInput"); // ʹ��Ψһ��ID

                // ����һ���ı�����������û��༭һ��ʵ��� tag
                if (ImGui::InputText("", buffer, sizeof(buffer))) 
                {
                    tag = std::string(buffer); // ����ʵ��� tag
                }

                ImGui::PopID(); // �ָ�ID״̬
                ImGui::TreePop();
            }
        }

        ImGui::Separator();

        if (entity.HasComponent<TransformComponent>()) {
            if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, u8"�任")) {
                auto& translate = entity.GetComponent<TransformComponent>().Translation;
                auto& scale = entity.GetComponent<TransformComponent>().Scale;
                auto& rot = entity.GetComponent<TransformComponent>().Rotation;

                DrawVec3Control(u8"λ��", translate);
                DrawVec3Control(u8"��ת", rot);
                DrawVec3Control(u8"����", scale, 1.0f);

                ImGui::TreePop();
            }
        }
    }
}