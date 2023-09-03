#include "krpch.h"
#include "UI.h"

#include "Kaesar/Utils/TransString.h"

namespace Kaesar {
    static bool scaleLock = true;

    void UI::DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
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
            ImGui::Checkbox("##L", &scaleLock);
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
        if (label == u8"����" && scaleLock)
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
        if (label == u8"����" && scaleLock)
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
        if (label == u8"����" && scaleLock)
        {
            values.x = values.z;
            values.y = values.z;
        }
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);

        ImGui::PopID();
    }

    bool UI::SliderFloat(const std::string& name, float* value, float min, float max)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::Text(name.c_str());
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

        return ImGui::SliderFloat(name.c_str(), value, min, max);
    }

    bool UI::DragFloat(const std::string& name, float* value, float speed, float min, float max)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::Text(name.c_str());
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

        return ImGui::DragFloat(name.c_str(), value, speed, min, max);
    }

    std::string UI::DrawIconFont(const char* name, const char* icon)
    {
        std::stringstream ss;
        std::string title = name;

        title = TransString::TBS(title);
        ss << icon << title;

        return ss.str();
    }

    void UI::Tooltip(const std::string& text)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 4, 6 });
        ImGui::BeginTooltip();
        ImGui::Text(text.c_str());
        ImGui::EndTooltip();
        ImGui::PopStyleVar();
    }
}