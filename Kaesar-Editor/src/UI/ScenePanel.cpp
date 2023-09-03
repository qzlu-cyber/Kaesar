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
        ImGui::Begin(u8"�������");
        ImGui::ShowStyleEditor(); // �� ImGui ��ʽ�༭�����Զ��� ImGui ��������
        ImGui::End();

        /// ====================== scene ========================
        ImGui::Begin((UI::DrawIconFont(" ����", ICON_FA_LIST_UL)).c_str());

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

        // �������������˵�ǰ�Ĵ��ڣ����ҵ�ǰ�Ĵ�������ͣ��
        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
            m_SelectionContext = {}; // ���ѡ�е�ʵ��

        // �������Ҽ�����˵�ǰ�Ĵ��ڣ�����һ������ʽ�����Ĳ˵�
        if (ImGui::BeginPopupContextWindow(0, 1, false))
        {
            if (ImGui::MenuItemEx(u8"�½�����", ICON_FA_FILE))
            {
                m_SelectionContext = m_Context->CreateEntity();
            }
            if (ImGui::BeginMenuEx(u8"���Ĭ������", ICON_FA_CUBE))
            {
                if (ImGui::MenuItemEx(u8"������", ICON_FA_CUBE))
                {
                    m_SelectionContext = *(m_Context->CreatePrimitive(PrimitiveType::Cube));
                }
                if (ImGui::MenuItemEx(u8"����", ICON_FA_CIRCLE))
                {
                    m_SelectionContext = *(m_Context->CreatePrimitive(PrimitiveType::Sphere));
                }
                if (ImGui::MenuItemEx(u8"ƽ��", ICON_FA_MAP))
                {
                    m_SelectionContext = *(m_Context->CreatePrimitive(PrimitiveType::Plane));
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenuEx(u8"��ӹ���", ICON_FA_LIGHTBULB))
            {
                if (ImGui::MenuItemEx(u8"ƽ�й�", ICON_FA_SUN))
                {
                    m_SelectionContext = *(m_Context->CreateLight(LightType::Directional));
                }
                if (ImGui::MenuItemEx(u8"���Դ", ICON_FA_LIGHTBULB))
                {
                    m_SelectionContext = *(m_Context->CreateLight(LightType::Point));
                }
                if (ImGui::MenuItemEx(u8"�۹�", ICON_FA_SPLOTCH))
                {
                    m_SelectionContext = *(m_Context->CreateLight(LightType::Spot));
                }
                ImGui::EndMenu();
            }

            ImGui::EndPopup();
        }

        ImGui::End();

        /// ===================== Properties =======================
        ImGui::Begin((UI::DrawIconFont(" ���", ICON_FA_SLIDERS_H).c_str()));

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

        // ȷ��������Ⱦ��ʾʵ������ڵ�ı�־�����ʵ�嵱ǰ�Ƿ�ѡ�У��Լ��ڵ��Ƿ�Ӧ�ڵ�����ͷʱչ��
        ImGuiTreeNodeFlags flags = ((m_SelectionContext == *entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth; // �ڵ�Ŀ�Ƚ���չ����ǰ�е������
        
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
        // �ڽ����г���һ�����ڵ㣬ʹ��ָ���ı�־�ͱ�ǩ�ı����˺������õĽ��ָʾ�ڵ��Ǵ򿪻��ǹر�
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)*entity, flags, UI::DrawIconFont(tag.Tag.c_str(), name).c_str());
        ImGui::PopStyleVar();

        if (ImGui::IsItemClicked()) // �Ƿ����˵�ǰ�����ڵ㣨ʵ�壩
        {
            m_SelectionContext = *entity; // ѡ�е�ǰ��ʵ��
        }

        // ʵ����ק����
        for (int n = 0; n < m_Context->m_Entities.size(); n++)
        {
            auto& item = m_Context->m_Entities[n];
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

        bool entityDeleted = false;
        if (ImGui::BeginPopupContextItem()) // ����Ҽ�����˵�ǰ�����ڵ㣨ʵ�壩
        {
            if (ImGui::MenuItemEx(u8"ɾ��", ICON_FA_TRASH)) // ����һ���˵�������û�ɾ��ʵ��
                entityDeleted = true;
            if (ImGui::MenuItemEx(u8"����", ICON_FA_COPY))
            {
                m_SelectionContext = *entity;
                m_EntityCreated = true;
            }

            ImGui::EndPopup();
        }

        if (opened) { // �����ǰ�����ڵ㣨ʵ�壩�Ǵ򿪵�
            ImGui::TreePop(); // �رյ�ǰ�����ڵ㣨ʵ�壩
        }

        if (entityDeleted)
        {
            if (m_SelectionContext == *entity)
                m_SelectionContext = {}; // ����ѡ�е�ʵ��
            m_Context->DestroyEntity(*entity); // ����ʵ��
        }
    }

    void ScenePanel::DrawComponents(Entity& entity)
    {
        static bool TagRemove = false;
        if (UI::DrawComponent<TagComponent>(UI::DrawIconFont(" ����", ICON_FA_PEN), entity, false, &TagRemove))
        {
            auto& tag = entity.GetComponent<TagComponent>().Tag;

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy_s(buffer, tag.c_str());

            // ����һ���ı�����������û��༭һ��ʵ��� tag
            if (ImGui::InputText(u8"����", buffer, sizeof(buffer)))
            {
                tag = std::string(buffer); // ����ʵ��� tag
            }

            ImGui::TreePop();

            ImGui::Separator();
        }

        static bool TransformRemove = false;
        if (UI::DrawComponent<TransformComponent>(UI::DrawIconFont(" �任", ICON_FA_PENCIL_RULER), entity, false, &TransformRemove))
        {
            TransformComponent& transformComponent = entity.GetComponent<TransformComponent>();
            UI::DrawVec3Control(u8"λ��", transformComponent.Translation);
            ImGui::Separator();
            glm::vec3 Rot = glm::degrees(transformComponent.Rotation);
            UI::DrawVec3Control(u8"��ת", Rot);
            transformComponent.Rotation = glm::radians(Rot);
            ImGui::Separator();
            UI::DrawVec3Control(u8"����", transformComponent.Scale, 1.0f);

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
        if (ImGui::Button(u8"������"))
            ImGui::OpenPopup("AddComponent");
        ImGui::PopStyleVar();

        if (ImGui::BeginPopup("AddComponent"))
        {
            if (ImGui::MenuItem(u8"���"))
            {
                if (!m_SelectionContext.HasComponent<CameraComponent>())
                    m_SelectionContext.AddComponent<CameraComponent>();
                else
                    KR_CORE_WARN("����Ѵ��ڣ�");

                ImGui::CloseCurrentPopup();
            }

            if (ImGui::MenuItem(u8"����"))
            {
                if (!m_SelectionContext.HasComponent<MeshComponent>())
                    m_SelectionContext.AddComponent<MeshComponent>();
                else
                    KR_CORE_WARN("����Ѵ��ڣ�");

                ImGui::CloseCurrentPopup();
            }

            if (ImGui::MenuItem(u8"����"))
            {
                if (!m_SelectionContext.HasComponent<MaterialComponent>())
                    m_SelectionContext.AddComponent<MaterialComponent>(m_Shaders.Get("GeometryPass"));
                else
                    KR_CORE_WARN("����Ѵ��ڣ�");

                ImGui::CloseCurrentPopup();
            }

            if (ImGui::MenuItem(u8"�ƹ�"))
            {
                if (!m_SelectionContext.HasComponent<LightComponent>())
                    m_SelectionContext.AddComponent<LightComponent>();
                else
                    KR_CORE_WARN("����Ѵ��ڣ�");

                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::PopItemWidth();
    }
}