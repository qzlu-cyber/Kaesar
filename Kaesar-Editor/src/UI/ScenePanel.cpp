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
        ImGui::ShowStyleEditor(); // 打开 ImGui 样式编辑器，自定义 ImGui 界面的外观

        /// ====================== scene ========================
        ImGui::Begin(u8"场景");

        for (auto entity : m_Context->m_Entities)
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
            if (ImGui::Button(u8"添加组件")) 
            {

            }
        }

        ImGui::End();
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
            auto item = m_Context->m_Entities[n];
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

                // 呈现一个文本输入框，允许用户编辑一个实体的 tag
                if (ImGui::InputText("", buffer, sizeof(buffer))) {
                    tag = std::string(buffer); // 更新实体的 tag
                }

                if (ImGui::IsItemClicked()) {
                    m_SelectionContext = entity; // 选中当前的实体
                }
                ImGui::TreePop();
            }
        }

        if (entity.HasComponent<TransformComponent>()) {

            if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, u8"变换")) {
                auto& translate = entity.GetComponent<TransformComponent>().Translation;
                auto& scale = entity.GetComponent<TransformComponent>().Scale;
                auto& rot = entity.GetComponent<TransformComponent>().Rotation;


                ImGui::TextColored(ImVec4(.8f, .1f, .5f, 1.0f), u8"位置");
                ImGui::DragFloat3("", glm::value_ptr(translate), 0.1f);

                ImGui::TextColored(ImVec4(.8f, .1f, .5f, 1.0f), u8"旋转");
                ImGui::DragFloat3("1", glm::value_ptr(rot), 0.5f, 0, 180);

                ImGui::TextColored(ImVec4(.8f, .1f, .5f, 1.0f), u8"缩放");
                ImGui::DragFloat3("2", glm::value_ptr(scale), 0.1f);

                ImGui::TreePop();
            }

        }
    }
}