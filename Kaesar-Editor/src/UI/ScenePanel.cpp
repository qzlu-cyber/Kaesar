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
        ImGui::ShowStyleEditor(); // �� ImGui ��ʽ�༭�����Զ��� ImGui ��������

        /// ====================== scene ========================
        ImGui::Begin(u8"����");

        for (auto& entity : m_Context->m_Entities)
        {
            DrawEntity(entity);
        }

        // �������������˵�ǰ�Ĵ��ڣ����ҵ�ǰ�Ĵ�������ͣ��
        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
            m_SelectionContext = {}; // ���ѡ�е�ʵ��

        // �������Ҽ�����˵�ǰ�Ĵ��ڣ�����һ������ʽ�����Ĳ˵�
        if (ImGui::BeginPopupContextWindow(0, 1, false))
        {
            if (ImGui::Selectable(u8"�½�����")) 
            {
                m_Context->CreateEntity();
            }
            ImGui::EndPopup();
        }

        ImGui::End();

        /// ===================== Properties =======================
        ImGui::Begin(u8"���");

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

    /// <summary>
    /// һ������ģ�壬������һ��ͳһ�ķ�ʽ�»��Ʋ�ͬ���͵�������û�����
    /// </summary>
    /// <typeparam name="T">�������</typeparam>
    /// <typeparam name="UIFunction">һ���ɵ��ö��������� UI �л��ƺͽ������������</typeparam>
    /// <param name="name">��������ƣ�������ʾ�� UI �еı���</param>
    /// <param name="entity">��ʾ��ǰʵ������ã�ͨ��������ÿ��Ի�ȡ�Ͳ���ʵ������</param>
    /// <param name="removable">��ʾ�Ƿ������Ƴ�������</param>
    /// <param name="uiFunction">һ���ɵ��ö��������� UI �л��ƺͽ������������</param>
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
            // ����һ�� UI �ڵ㣬��ʾ��������ݣ����� UI ����ʾһ����չ���Ľڵ�
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
                    if (ImGui::MenuItem(u8"ɾ�����")) // ����û�ѡ�������ѡ��
                        removeComponent = true;
                }
                ImGui::EndPopup();
            }
            // ��� UI �ڵ��Ƿ�չ��
            if (open)
            {
                uiFunction(component); // ����ڵ㱻չ�����͵��ô���� UI ������uiFunction������������ľ�������
                ImGui::TreePop(); // �ر� UI �ڵ�
            }

            if (removeComponent)
                entity.RemoveComponent<T>();
        }
    }

    void ScenePanel::DrawEntity(std::shared_ptr<Entity>& entity)
    {
        auto& tag = (*entity).GetComponent<TagComponent>();

        // ȷ��������Ⱦ��ʾʵ������ڵ�ı�־�����ʵ�嵱ǰ�Ƿ�ѡ�У��Լ��ڵ��Ƿ�Ӧ�ڵ�����ͷʱչ��
        ImGuiTreeNodeFlags flags = ((m_SelectionContext == *entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth; // �ڵ�Ŀ�Ƚ���չ����ǰ�е������
        // �ڽ����г���һ�����ڵ㣬ʹ��ָ���ı�־�ͱ�ǩ�ı����˺������õĽ��ָʾ�ڵ��Ǵ򿪻��ǹر�
        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)*entity, flags, tag.Tag.c_str());
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
            if (ImGui::Selectable(u8"ɾ��ʵ��")) // ����һ���˵�������û�ɾ��ʵ��
                entityDeleted = true;

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

    void ScenePanel::DrawComponents(Entity entity)
    {
        DrawComponent<TagComponent>(u8"����", entity, false, [](TagComponent& component)
            {
                auto& tag = component.Tag;

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
            });

        ImGui::Separator();

        DrawComponent<TransformComponent>(u8"�任", entity, false, [this](TransformComponent& component)
            {
                DrawVec3Control(u8"λ��", component.Translation);
                ImGui::Separator();
                DrawVec3Control(u8"��ת", component.Rotation);
                ImGui::Separator();
                DrawVec3Control(u8"����", component.Scale, 1.0f);
            });

        ImGui::Separator();

        DrawComponent<CameraComponent>(u8"���", entity, true, [this](CameraComponent& component)
            {
                SceneCamera& camera = component.Camera;

                ImGui::Checkbox(u8"�����", &component.Primary);

                const char* projectionType[] = { "Perspective", "Orthographic" };
                const char* currentProjectionType = projectionType[(int)camera.GetProjectionType()];
                // �����˵�������ѡ�������ͶӰ����
                if (ImGui::BeginCombo(u8"ͶӰ��ʽ", currentProjectionType)) // ��ȡ��ǰ�����ͶӰ���Ͳ�����Ϊ�����˵��ĵ�ǰѡ����
                {
                    // ʹ��ѭ����������ѡ�������ѡ��ĳ����ʱ���������ͶӰ����
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
                    if (ImGui::DragFloat(u8"��Ұ", &perspectiveFov))
                        camera.SetPerspectiveFOV(glm::radians(perspectiveFov));

                    float perspectiveNear = camera.GetPerspectiveNearClip();
                    if (ImGui::DragFloat(u8"��ƽ��", &perspectiveNear))
                        camera.SetPerspectiveNearClip(perspectiveNear);

                    float perspectiveFar = camera.GetPerspectiveFarClip();
                    if (ImGui::DragFloat(u8"Զƽ��", &perspectiveFar))
                        camera.SetPerspectiveFarClip(perspectiveFar);
                }

                if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
                {
                    float orthoSize = camera.GetOrthographicSize();
                    if (ImGui::DragFloat(u8"�ߴ�", &orthoSize))
                        camera.SetOrthographicSize(orthoSize);

                    float orthoNear = camera.GetOrthographicNearClip();
                    if (ImGui::DragFloat(u8"��ƽ��", &orthoNear))
                        camera.SetOrthographicNearClip(orthoNear);

                    float orthoFar = camera.GetOrthographicFarClip();
                    if (ImGui::DragFloat(u8"Զƽ��", &orthoFar))
                        camera.SetOrthographicFarClip(orthoFar);

                    ImGui::Checkbox(u8"�̶���߱�", &component.FixedAspectRatio);
                }
            });

        ImGui::Separator();

        if (entity.HasComponent<MeshComponent>()) {
            auto& tag = entity.GetComponent<MeshComponent>().path;

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy_s(buffer, tag.c_str());

            if (ImGui::InputText(u8"·��", buffer, sizeof(buffer)))
            {
                tag = std::string(buffer);
            }

            ImGui::SameLine();

            if (ImGui::Button(u8"ѡ��"))
            {
                auto dir = std::filesystem::current_path();
                std::optional<std::string> path = Kaesar::FileDialogs::OpenFile("ģ���ļ� (*.obj)\0*.obj\0");
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

            ImGui::EndPopup();
        }

        ImGui::PopItemWidth();
    }
}