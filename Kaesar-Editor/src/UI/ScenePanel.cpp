#include "krpch.h"
#include "ScenePanel.h"

#include "Kaesar/Renderer/Model.h"
#include "Kaesar/Utils/PlatformUtils.h"

#include <filesystem>
#include <cstring>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

namespace Kaesar {
    ScenePanel::ScenePanel(const std::shared_ptr<Scene>& scene)
    {
        m_Context = scene;
        m_SelectionContext = {};
    }

    void ScenePanel::OnImGuiRender()
    {
        ImGui::ShowStyleEditor(); // 打开 ImGui 样式编辑器，自定义 ImGui 界面的外观

        /// ====================== scene ========================
        ImGui::Begin(u8"场景");

        for (auto& entity : m_Context->m_Entities)
        {
            DrawEntity(entity);
        }

        // 如果鼠标左键点击了当前的窗口，并且当前的窗口是悬停的
        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
            m_SelectionContext = {}; // 清空选中的实体

        // 如果鼠标右键点击了当前的窗口，开启一个弹出式上下文菜单
        if (ImGui::BeginPopupContextWindow(0, 1, false))
        {
            if (ImGui::Selectable(u8"新建物体")) 
            {
                m_Context->CreateEntity();
            }
            ImGui::EndPopup();
        }

        ImGui::End();

        /// ===================== Properties =======================
        ImGui::Begin(u8"组件");

        if (m_SelectionContext)
        {
            DrawComponents(m_SelectionContext);
            ImGui::SetNextItemWidth(-1);
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

    /// <summary>
    /// 一个函数模板，允许在一个统一的方式下绘制不同类型的组件的用户界面
    /// </summary>
    /// <typeparam name="T">组件类型</typeparam>
    /// <typeparam name="UIFunction">一个可调用对象，用于在 UI 中绘制和交互组件的内容</typeparam>
    /// <param name="name">组件的名称，用于显示在 UI 中的标题</param>
    /// <param name="entity">表示当前实体的引用，通过这个引用可以获取和操作实体的组件</param>
    /// <param name="removable">表示是否允许移除这个组件</param>
    /// <param name="uiFunction">一个可调用对象，用于在 UI 中绘制和交互组件的内容</param>
    template<typename T, typename UIFunction>
    void DrawComponent(const std::string& name, Entity entity, bool removable, UIFunction uiFunction)
    {
        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed
            | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap
            | ImGuiTreeNodeFlags_FramePadding;

        if (entity.HasComponent<T>())
        {
            auto& component = entity.GetComponent<T>();

            ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            // 创建一个 UI 节点，表示组件的内容，会在 UI 中显示一个可展开的节点
            bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
            ImGui::PopStyleVar();
            ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
            if (ImGui::Button("...", ImVec2{ lineHeight, lineHeight }))
            {
                ImGui::OpenPopup("ComponentSettings");
            }

            bool removeComponent = false;
            if (ImGui::BeginPopup("ComponentSettings"))
            {
                if (removable) 
                {
                    if (ImGui::MenuItem(u8"删除组件")) // 如果用户选择了这个选项
                        removeComponent = true;
                }
                ImGui::EndPopup();
            }
            // 检查 UI 节点是否被展开
            if (open)
            {
                uiFunction(component); // 如果节点被展开，就调用传入的 UI 函数（uiFunction）来绘制组件的具体内容
                ImGui::TreePop(); // 关闭 UI 节点
            }

            if (removeComponent)
                entity.RemoveComponent<T>();
        }
    }

    void ScenePanel::DrawEntity(std::shared_ptr<Entity>& entity)
    {
        auto& tag = (*entity).GetComponent<TagComponent>();

        // 确定用于渲染表示实体的树节点的标志。检查实体当前是否被选中，以及节点是否应在单击箭头时展开
        ImGuiTreeNodeFlags flags = ((m_SelectionContext == *entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth; // 节点的宽度将扩展到当前行的最大宽度
        // 在界面中呈现一个树节点，使用指定的标志和标签文本。此函数调用的结果指示节点是打开还是关闭
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)*entity, flags, tag.Tag.c_str());
        if (ImGui::IsItemClicked()) // 是否点击了当前的树节点（实体）
        {
            m_SelectionContext = *entity; // 选中当前的实体
        }

        // 实现拖拽排序
        for (int n = 0; n < m_Context->m_Entities.size(); n++)
        {
            auto& item = m_Context->m_Entities[n];
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

        bool entityDeleted = false;
        if (ImGui::BeginPopupContextItem()) // 如果右键点击了当前的树节点（实体）
        {
            if (ImGui::Selectable(u8"删除实体")) // 呈现一个菜单项，允许用户删除实体
                entityDeleted = true;

            ImGui::EndPopup();
        }

        if (opened) { // 如果当前的树节点（实体）是打开的
            ImGui::TreePop(); // 关闭当前的树节点（实体）
        }

        if (entityDeleted)
        {
            if (m_SelectionContext == *entity)
                m_SelectionContext = {}; // 重置选中的实体
            m_Context->DestroyEntity(*entity); // 销毁实体
        }
    }

    void ScenePanel::DrawComponents(Entity entity)
    {
        DrawComponent<TagComponent>(u8"名称", entity, false, [](TagComponent& component)
            {
                auto& tag = component.Tag;

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
            });

        ImGui::Separator();

        DrawComponent<TransformComponent>(u8"变换", entity, false, [this](TransformComponent& component)
            {
                DrawVec3Control(u8"位置", component.Translation);
                ImGui::Separator();
                DrawVec3Control(u8"旋转", component.Rotation);
                ImGui::Separator();
                DrawVec3Control(u8"缩放", component.Scale, 1.0f);
            });

        ImGui::Separator();

        DrawComponent<CameraComponent>(u8"相机", entity, true, [this](CameraComponent& component)
            {
                SceneCamera& camera = component.Camera;

                ImGui::Checkbox(u8"主相机", &component.Primary);

                const char* projectionType[] = { "Perspective", "Orthographic" };
                const char* currentProjectionType = projectionType[(int)camera.GetProjectionType()];
                // 下拉菜单，用于选择相机的投影类型
                if (ImGui::BeginCombo(u8"投影方式", currentProjectionType)) // 获取当前相机的投影类型并设置为下拉菜单的当前选择项
                {
                    // 使用循环绘制两个选择项，并在选择某个项时更新相机的投影类型
                    for (int i = 0; i < 2; i++)
                    {
                        bool isSelected = currentProjectionType == projectionType[i];
                        if (ImGui::Selectable(projectionType[i], isSelected))
                        {
                            currentProjectionType = projectionType[i];
                            camera.SetProjectionType((SceneCamera::ProjectionType)i);
                        }

                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }

                    ImGui::EndCombo();
                }

                if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
                {
                    float perspectiveFov = glm::degrees(camera.GetPerspectiveFOV());
                    if (ImGui::DragFloat(u8"视野", &perspectiveFov))
                        camera.SetPerspectiveFOV(glm::radians(perspectiveFov));

                    float perspectiveNear = camera.GetPerspectiveNearClip();
                    if (ImGui::DragFloat(u8"近平面", &perspectiveNear))
                        camera.SetPerspectiveNearClip(perspectiveNear);

                    float perspectiveFar = camera.GetPerspectiveFarClip();
                    if (ImGui::DragFloat(u8"远平面", &perspectiveFar))
                        camera.SetPerspectiveFarClip(perspectiveFar);
                }

                if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
                {
                    float orthoSize = camera.GetOrthographicSize();
                    if (ImGui::DragFloat(u8"尺寸", &orthoSize))
                        camera.SetOrthographicSize(orthoSize);

                    float orthoNear = camera.GetOrthographicNearClip();
                    if (ImGui::DragFloat(u8"近平面", &orthoNear))
                        camera.SetOrthographicNearClip(orthoNear);

                    float orthoFar = camera.GetOrthographicFarClip();
                    if (ImGui::DragFloat(u8"远平面", &orthoFar))
                        camera.SetOrthographicFarClip(orthoFar);

                    ImGui::Checkbox(u8"固定宽高比", &component.FixedAspectRatio);
                }
            });

        ImGui::Separator();

        if (entity.HasComponent<MeshComponent>()) {
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
                std::optional<std::string> path = Kaesar::FileDialogs::OpenFile("模型文件 (*.obj)\0*.obj\0");
                if (path) {
                    auto filepath = path->substr(dir.string().size());
                    tag = filepath;
                    entity.GetComponent<MeshComponent>().model = Model(filepath);
                }
            }
        }

        ImGui::Separator();
        float buttonSz = 100;
        ImGui::PushItemWidth(buttonSz);

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImGui::Dummy({ 0, 10 });
        ImGui::NewLine();
        ImGui::SameLine(ImGui::GetContentRegionAvail().x / 2.0f - buttonSz / 2.0f);

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 100 });
        if (ImGui::Button(u8"添加组件"))
            ImGui::OpenPopup("AddComponent");
        ImGui::PopStyleVar();

        if (ImGui::BeginPopup("AddComponent"))
        {
            if (ImGui::MenuItem(u8"相机"))
            {
                if (!m_SelectionContext.HasComponent<CameraComponent>())
                    m_SelectionContext.AddComponent<CameraComponent>();
                else
                    KR_CORE_WARN("组件已存在！");

                ImGui::CloseCurrentPopup();
            }

            if (ImGui::MenuItem(u8"网格"))
            {
                if (!m_SelectionContext.HasComponent<MeshComponent>())
                    m_SelectionContext.AddComponent<MeshComponent>();
                else
                    KR_CORE_WARN("组件已存在！");

                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::PopItemWidth();
    }
}