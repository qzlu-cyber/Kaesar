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
        ImGui::ShowStyleEditor(); // 打开 ImGui 样式编辑器，自定义 ImGui 界面的外观

        /// ====================== scene ========================
        ImGui::Begin(u8"场景");

        for (auto& entity : m_Context->m_Entities)
        {
            DrawEntity(*entity);
        }

        // 如果鼠标左键点击了当前的窗口，并且当前的窗口是悬停的
        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
            m_SelectionContext = {}; // 清空选中的实体

        // 如果鼠标右键点击了当前的窗口，开启一个弹出式上下文菜单
        if (ImGui::BeginPopupContextWindow(0, 1))
        {
            if (ImGui::Selectable(u8"新建物体")) {
                m_Context->CreateEntity();
            }
            ImGui::EndPopup();
        }

        ImGui::End();

        /// ===================== Properties =======================
        ImGui::Begin(u8"属性");

        if (m_SelectionContext)
        {
            DrawComponents(m_SelectionContext);
            ImGui::SetNextItemWidth(-1);
            // TODO: Add Component
            if (ImGui::Button(u8"添加属性")) 
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
            ImGui::Checkbox("##L", &m_ScaleLock);
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
        if (label == u8"缩放" && m_ScaleLock)
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
        if (label == u8"缩放" && m_ScaleLock)
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
        if (label == u8"缩放" && m_ScaleLock)
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

        // 确定用于渲染表示实体的树节点的标志。检查实体当前是否被选中，以及节点是否应在单击箭头时展开
        ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

        // 在界面中呈现一个树节点，使用指定的标志和标签文本。此函数调用的结果指示节点是打开还是关闭
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.Tag.c_str());
        if (ImGui::IsItemClicked()) { // 是否点击了当前的树节点（实体）
            m_SelectionContext = entity; // 选中当前的实体
        }

        // 实现拖拽排序
        for (int n = 0; n < m_Context->m_Entities.size(); n++)
        {
            auto&item = m_Context->m_Entities[n];
            // 检查是否有一个 ImGui 项目被拖动并且不是悬停的
            if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
            {
                int n_next = n + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1); // 根据鼠标拖拽的方向计算出下一个位置的索引
                if (n_next >= 0 && n_next < m_Context->m_Entities.size()) // 边界检查，交换两个实体的位置
                {
                    m_Context->m_Entities[n] = m_Context->m_Entities[n_next];
                    m_Context->m_Entities[n_next] = item;
                    ImGui::ResetMouseDragDelta(); // 重置鼠标拖拽的变化量
                }
            }
        }

        if (opened) { // 如果当前的树节点（实体）是打开的
            ImGui::TreePop(); // 关闭当前的树节点（实体）
        }
    }

    void ScenePanel::DrawComponents(Entity entity)
    {
        if (entity.HasComponent<TagComponent>()) {
            if (ImGui::TreeNodeEx((void*)typeid(TagComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, u8"名称")) {
                auto& tag = entity.GetComponent<TagComponent>().Tag;

                char buffer[256];
                memset(buffer, 0, sizeof(buffer));
                strcpy_s(buffer, tag.c_str());

                ImGui::PushID("TagInput"); // 使用唯一的ID

                // 呈现一个文本输入框，允许用户编辑一个实体的 tag
                if (ImGui::InputText("", buffer, sizeof(buffer))) 
                {
                    tag = std::string(buffer); // 更新实体的 tag
                }

                ImGui::PopID(); // 恢复ID状态
                ImGui::TreePop();
            }
        }

        ImGui::Separator();

        if (entity.HasComponent<TransformComponent>()) {
            if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, u8"变换")) {
                auto& translate = entity.GetComponent<TransformComponent>().Translation;
                auto& scale = entity.GetComponent<TransformComponent>().Scale;
                auto& rot = entity.GetComponent<TransformComponent>().Rotation;

                DrawVec3Control(u8"位置", translate);
                DrawVec3Control(u8"旋转", rot);
                DrawVec3Control(u8"缩放", scale, 1.0f);

                ImGui::TreePop();
            }
        }
    }
}