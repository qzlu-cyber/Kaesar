#include "krpch.h"
#include "ScenePanel.h"
#include "UI.h"

#include "Kaesar/Renderer/Model.h"
#include "Kaesar/Utils/PlatformUtils.h"

#include <cstring>
#include <glm/gtc/type_ptr.hpp>

namespace Kaesar {
    ScenePanel::ScenePanel(const std::shared_ptr<Scene>& scene)
    {
        m_Context = scene;

        m_Shaders = SceneRenderer::GetShaderLibrary();
        int size = m_Shaders.GetShaders().size();
        int index = 0;
        for (auto&& [name, shader] : m_Shaders.GetShaders())
        {
            m_ShaderNames.push_back(name);
        }

        m_MeshPanel = std::make_shared<MeshPanel>();
        m_MaterialPanel = std::make_shared<MaterialPanel>();
        m_LightPanel = std::make_shared<LightPanel>();
        m_CameraPanel = std::make_shared<CameraPanel>();

        m_SelectionContext = {};
    }

    void ScenePanel::OnImGuiRender()
    {
        ImGui::Begin(u8"更改外观");
        ImGui::ShowStyleEditor(); // 打开 ImGui 样式编辑器，自定义 ImGui 界面的外观
        ImGui::End();

        /// ====================== scene ========================
        ImGui::Begin((UI::DrawIconFont(" 场景", ICON_FA_LIST_UL)).c_str());

        for (auto& entity : m_Context->m_Entities)
        {
            if (entity)
                DrawEntity(entity);
        }

        if (m_EntityCreated) 
        {
            m_Context->CreateEntity(m_SelectionContext);
            m_EntityCreated = false;
        }

        // 如果鼠标左键点击了当前的窗口，并且当前的窗口是悬停的
        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
            m_SelectionContext = {}; // 清空选中的实体

        // 如果鼠标右键点击了当前的窗口，开启一个弹出式上下文菜单
        if (ImGui::BeginPopupContextWindow(0, 1, false))
        {
            if (ImGui::MenuItemEx(u8"新建物体", ICON_FA_FILE))
            {
                m_SelectionContext = m_Context->CreateEntity();
            }
            if (ImGui::BeginMenuEx(u8"添加默认物体", ICON_FA_CUBE))
            {
                if (ImGui::MenuItemEx(u8"立方体", ICON_FA_CUBE))
                {
                    m_SelectionContext = *(m_Context->CreatePrimitive(PrimitiveType::Cube));
                }
                if (ImGui::MenuItemEx(u8"球体", ICON_FA_CIRCLE))
                {
                    m_SelectionContext = *(m_Context->CreatePrimitive(PrimitiveType::Sphere));
                }
                if (ImGui::MenuItemEx(u8"平面", ICON_FA_MAP))
                {
                    m_SelectionContext = *(m_Context->CreatePrimitive(PrimitiveType::Plane));
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenuEx(u8"添加光照", ICON_FA_LIGHTBULB))
            {
                if (ImGui::MenuItemEx(u8"平行光", ICON_FA_SUN))
                {
                    m_SelectionContext = *(m_Context->CreateLight(LightType::Directional));
                }
                if (ImGui::MenuItemEx(u8"点光源", ICON_FA_LIGHTBULB))
                {
                    m_SelectionContext = *(m_Context->CreateLight(LightType::Point));
                }
                if (ImGui::MenuItemEx(u8"聚光", ICON_FA_SPLOTCH))
                {
                    m_SelectionContext = *(m_Context->CreateLight(LightType::Spot));
                }
                ImGui::EndMenu();
            }

            ImGui::EndPopup();
        }

        ImGui::End();

        /// ===================== Properties =======================
        ImGui::Begin((UI::DrawIconFont(" 组件", ICON_FA_SLIDERS_H).c_str()));

        if (m_SelectionContext)
        {
            DrawComponents(m_SelectionContext);
            ImGui::SetNextItemWidth(-1);
        }

        ImGui::End();
    }

    void ScenePanel::DrawEntity(std::shared_ptr<Entity>& entity)
    {
        auto& tag = (*entity).GetComponent<TagComponent>();

        // 确定用于渲染表示实体的树节点的标志。检查实体当前是否被选中，以及节点是否应在单击箭头时展开
        ImGuiTreeNodeFlags flags = ((m_SelectionContext == *entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth; // 节点的宽度将扩展到当前行的最大宽度
        
        const char* name = "";
        if (entity->HasComponent<MeshComponent>())
        {
            name = ICON_FA_CUBE;
        }
        if (entity->HasComponent<LightComponent>())
        {
            name = ICON_FA_LIGHTBULB;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 2 });
        // 在界面中呈现一个树节点，使用指定的标志和标签文本。此函数调用的结果指示节点是打开还是关闭
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)*entity, flags, UI::DrawIconFont(tag.Tag.c_str(), name).c_str());
        ImGui::PopStyleVar();

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
            if (ImGui::MenuItemEx(u8"删除", ICON_FA_TRASH)) // 呈现一个菜单项，允许用户删除实体
                entityDeleted = true;
            if (ImGui::MenuItemEx(u8"复制", ICON_FA_COPY))
            {
                m_SelectionContext = *entity;
                m_EntityCreated = true;
            }

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

    void ScenePanel::DrawComponents(Entity& entity)
    {
        static bool TagRemove = false;
        if (UI::DrawComponent<TagComponent>(UI::DrawIconFont(" 名称", ICON_FA_PEN), entity, false, &TagRemove))
        {
            auto& tag = entity.GetComponent<TagComponent>().Tag;

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy_s(buffer, tag.c_str());

            // 呈现一个文本输入框，允许用户编辑一个实体的 tag
            if (ImGui::InputText(u8"名称", buffer, sizeof(buffer)))
            {
                tag = std::string(buffer); // 更新实体的 tag
            }

            ImGui::TreePop();

            ImGui::Separator();
        }

        static bool TransformRemove = false;
        if (UI::DrawComponent<TransformComponent>(UI::DrawIconFont(" 变换", ICON_FA_PENCIL_RULER), entity, false, &TransformRemove))
        {
            TransformComponent& transformComponent = entity.GetComponent<TransformComponent>();
            UI::DrawVec3Control(u8"位置", transformComponent.Translation);
            ImGui::Separator();
            glm::vec3 Rot = glm::degrees(transformComponent.Rotation);
            UI::DrawVec3Control(u8"旋转", Rot);
            transformComponent.Rotation = glm::radians(Rot);
            ImGui::Separator();
            UI::DrawVec3Control(u8"缩放", transformComponent.Scale, 1.0f);

            ImGui::TreePop();

            ImGui::Separator();
        }

        m_MeshPanel->DrawMesh(entity);
        m_MaterialPanel->DrawMaterial(entity);
        m_LightPanel->DrawLight(entity);
        m_CameraPanel->DrawCamera(entity);

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

            if (ImGui::MenuItem(u8"材质"))
            {
                if (!m_SelectionContext.HasComponent<MaterialComponent>())
                    m_SelectionContext.AddComponent<MaterialComponent>(m_Shaders.Get("GeometryPass"));
                else
                    KR_CORE_WARN("组件已存在！");

                ImGui::CloseCurrentPopup();
            }

            if (ImGui::MenuItem(u8"灯光"))
            {
                if (!m_SelectionContext.HasComponent<LightComponent>())
                    m_SelectionContext.AddComponent<LightComponent>();
                else
                    KR_CORE_WARN("组件已存在！");

                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::PopItemWidth();
    }
}