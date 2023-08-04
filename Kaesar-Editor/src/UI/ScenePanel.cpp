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
        ImGui::ShowStyleEditor(); // 打开 ImGui 样式编辑器，自定义 ImGui 界面的外观
        ImGui::Begin(u8"场景");

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

        // 确定用于渲染表示实体的树节点的标志。检查实体当前是否被选中，以及节点是否应在单击箭头时展开
        ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

        // 在界面中呈现一个树节点，使用指定的标志和标签文本。此函数调用的结果指示节点是打开还是关闭
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.Tag.c_str());
        if (ImGui::IsItemClicked()) { // 是否点击了当前的树节点（实体）
            m_SelectionContext = entity; // 选中当前的实体
        }

        if (opened) { // 如果当前的树节点（实体）是打开的
            ImGui::TreePop(); // 关闭当前的树节点（实体）
        }
    }
}