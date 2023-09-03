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

        // 在两列布局中绘制向量控件
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth); // 设置第一列的宽度
        // 绘制标签
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 5 }); // 设置控件之间的间距
        ImGui::Text(label.c_str());

        // 绘制 checkbox
        if (label == u8"缩放")
        {
            ImGui::SameLine();
            // 调整单选框距离左边的间距
            float marginLeft = 10.0f;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + marginLeft);
            // 保存当前的光标位置
            float startPosX = ImGui::GetCursorPosX();
            // 设置单选框的宽度
            float checkboxWidth = 20.0f;
            ImGui::SetNextItemWidth(checkboxWidth);
            // 绘制一个自定义的单选框
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0)); // 去除内边距
            ImGui::Checkbox("##L", &scaleLock);
            ImGui::PopStyleVar();
            // 恢复光标位置
            ImGui::SetCursorPosX(startPosX + checkboxWidth);
            // 恢复宽度设置
            ImGui::SetNextItemWidth(-1); // 重置宽度设置
        }

        ImGui::PopStyleVar();
        ImGui::NextColumn(); // 切换到下一列

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f; // 计算行高
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight }; // 设置按钮的大小

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth()); // 设置下面三个控件的宽度
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 5 }); // 设置控件之间的间距

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
        if (label == u8"缩放" && scaleLock)
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
        if (label == u8"缩放" && scaleLock)
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
        if (label == u8"缩放" && scaleLock)
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