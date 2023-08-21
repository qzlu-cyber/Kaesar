#include "krpch.h"
#include "ScenePanel.h"

#include "Kaesar/Renderer/Model.h"
#include "Kaesar/Utils/PlatformUtils.h"

#include <filesystem>
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

        m_EmptyTexture = Texture2D::Create("assets/models/cube/default.png", 0);
        m_TextureId = reinterpret_cast<void*>(m_EmptyTexture->GetRendererID());
        m_SelectedShader = "basic";

        m_SelectionContext = {};
    }

    void ScenePanel::OnImGuiRender()
    {
        ImGui::Begin(u8"�������");
        ImGui::ShowStyleEditor(); // �� ImGui ��ʽ�༭�����Զ��� ImGui ��������
        ImGui::End();

        /// ====================== scene ========================
        ImGui::Begin(u8"����");

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

    static std::string LightTypeToLightName(LightType type)
    {
        if (type == LightType::Directional)
            return u8"ƽ�й�";
        if (type == LightType::Point)
            return u8"���Դ";
        if (type == LightType::Spot)
            return u8"�۹�";
        return "";
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

    /// <summary>
    /// һ������ģ�壬������һ��ͳһ�ķ�ʽ�»��Ʋ�ͬ���͵�������û�����
    /// </summary>
    /// <typeparam name="T">�������</typeparam>
    /// <param name="name">�������</param>
    /// <param name="entity">Ҫ���Ƶĵ�ǰʵ��</param>
    /// <param name="removable">������Ƿ���Ƴ�</param>
    /// <param name="removed">����Ƿ��Ƴ�</param>
    /// <returns>��������һ������ֵ����ʾ���ڵ��Ƿ�չ��</returns>
    template<typename T>
    static bool DrawComponent(const std::string& name, Entity entity, bool removable, bool* removed)
    {
        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed
            | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap
            | ImGuiTreeNodeFlags_FramePadding;

        // �жϸ���ʵ���Ƿ�ӵ��ָ�����͵����������ǣ���ô�ͼ�������������
        if (entity.HasComponent<T>())
        {
            auto& component = entity.GetComponent<T>();
            ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
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
                if (removable) {
                    if (ImGui::MenuItem(u8"ɾ�����"))
                        removeComponent = true;
                }
                ImGui::EndPopup();
            }

            if (removeComponent)
                *removed = true;

            return open;
        }
        return false;
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
            if (ImGui::Selectable(u8"ɾ��")) // ����һ���˵�������û�ɾ��ʵ��
                entityDeleted = true;
            if (ImGui::Selectable(u8"����"))
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

    void ScenePanel::DrawComponents(Entity entity)
    {
        static bool TagRemove = false;
        if (DrawComponent<TagComponent>(u8"����", entity, false, &TagRemove))
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
        if (DrawComponent<TransformComponent>(u8"�任", entity, false, &TransformRemove))
        {
            TransformComponent& transformComponent = entity.GetComponent<TransformComponent>();
            DrawVec3Control(u8"λ��", transformComponent.Translation);
            ImGui::Separator();
            glm::vec3 Rot = glm::degrees(transformComponent.Rotation);
            DrawVec3Control(u8"��ת", Rot);
            transformComponent.Rotation = glm::radians(Rot);
            ImGui::Separator();
            DrawVec3Control(u8"����", transformComponent.Scale, 1.0f);

            ImGui::TreePop();

            ImGui::Separator();
        }

        static bool MaterialRemove = false;
        if (DrawComponent<MaterialComponent>(u8"����", entity, true, &MaterialRemove))
        {
            auto& materialComponent = entity.GetComponent<MaterialComponent>();

            ImGui::Separator();
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, 80);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
            ImGui::Text("Shader\0");

            ImGui::PopStyleVar();
            ImGui::NextColumn();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            
            ImGui::Text("PBR shader\0");
            ImGui::PopItemWidth();
            ImGui::Columns(1);
            ImGui::Separator();
            std::vector<Sampler>& samplers = materialComponent.material->GetSamplers();
            auto& materialTextures = materialComponent.material->GetTextures();
            for (auto& sampler : samplers)
            {
                ImGui::PushID(sampler.name.c_str());
                int frame_padding = -1 + 0;                           // -1 == uses default padding (style.FramePadding)
                ImVec2 size = ImVec2(64.0f, 64.0f);                  // Size of the image we want to make visible
                ImGui::Text(sampler.name.c_str());
                m_TextureId = reinterpret_cast<void*>(m_EmptyTexture->GetRendererID());
                auto& texture = materialComponent.material->GetTexture(sampler);
                if (texture)
                {
                    m_TextureId = reinterpret_cast<void*>(texture->GetRendererID());
                }

                ImGui::SameLine();
                ImGui::Checkbox(u8"����", &sampler.isUsed);

                //Diffuse
                if (sampler.binding == 0)
                {
                    static glm::vec4 color;
                    ImGui::ColorEdit4("Color", glm::value_ptr(color));
                    materialComponent.shader->Bind();
                    materialComponent.shader->SetFloat4("pc.material.color", color);
                    materialComponent.shader->Unbind();
                }

                if (ImGui::ImageButton(m_TextureId, size, ImVec2{ 0, 1 }, ImVec2{ 1, 0 })) 
                {
                    auto path = FileDialogs::OpenFile("Kaesar Texture (*.*)\0*.*\0");
                    if (path) 
                    {
                        // ��Ƕ����Ĭ��Ϊ��������ͼ���󶨵�Ϊ 0 ������������ͼ����������Ϊ sRGB ��ɫ�ռ�
                        materialTextures[sampler.binding] = Texture2D::Create(*path, 0, sampler.binding == 0);
                    }
                }
                ImGui::PopID();
            }
            ImGui::TreePop();

            if (MaterialRemove) {
                entity.RemoveComponent<MaterialComponent>();
                MaterialRemove = false;
            }

            ImGui::Separator();
        }

        static bool CameraRemove = false;
        if (DrawComponent<CameraComponent>(u8"���", entity, true, &CameraRemove))
        {
            CameraComponent& cameraComponent = entity.GetComponent<CameraComponent>();
            SceneCamera& camera = cameraComponent.Camera;

            ImGui::Checkbox(u8"�����", &cameraComponent.Primary);

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

                ImGui::Checkbox(u8"�̶���߱�", &cameraComponent.FixedAspectRatio);
            }

            ImGui::TreePop();

            if (CameraRemove)
            {
                entity.RemoveComponent<CameraComponent>();
                CameraRemove = false;
            }

            ImGui::Separator();
        }

        static bool MeshRemove = false;
        if (DrawComponent<MeshComponent>(u8"ģ��", entity, true, &MeshRemove))
        {
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
                if (path)
                {
                    std::string filepath;
                    if (path->find(dir.string()) != std::string::npos)
                    {
                        filepath = path->substr(dir.string().size());
                    }
                    else
                    {
                        filepath = *path;
                    }
                    tag = filepath;
                    entity.GetComponent<MeshComponent>().model = Model(filepath);
                }
            }

            ImGui::TreePop();
            if (MeshRemove) {
                entity.RemoveComponent<MeshComponent>();
                MeshRemove = false;
            }
        }

        static bool LightRemove = false;
        if (DrawComponent<LightComponent>(u8"�ƹ�", entity, true, &LightRemove))
        {
            auto& lightComponent = entity.GetComponent<LightComponent>();
            auto& transformComponent = entity.GetComponent<TransformComponent>();

            ImGui::Separator();
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, 80);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
            ImGui::Text(u8"�ƹ�����\0");

            ImGui::PopStyleVar();
            ImGui::NextColumn();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            std::string label = LightTypeToLightName(lightComponent.type);

            static int item_current_idx = 0;
            const char* combo_label = label.c_str();
            if (ImGui::BeginCombo(u8"##�ƹ�", combo_label))
            {
                for (int n = 0; n < 3; n++)
                {
                    const bool is_selected = (item_current_idx == n);

                    if (ImGui::Selectable(LightTypeToLightName((LightType)n).c_str(), is_selected)) 
                    {
                        lightComponent.type = (LightType)n;
                        if (lightComponent.type == LightType::Point) 
                        {
                            lightComponent.light = std::make_shared<PointLight>();
                        }
                        if (lightComponent.type == LightType::Directional)
                        {
                            lightComponent.light = std::make_shared<DirectionalLight>();
                        }
                        if (lightComponent.type == LightType::Spot) 
                        {
                            lightComponent.light = std::make_shared<SpotLight>();
                        }
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();
            ImGui::Columns(1);

            ImGui::Separator();

            auto& color4 = glm::vec4(lightComponent.light->GetColor(), 1);

            ImGui::SetNextItemWidth(60);
            ImGui::Text(u8"��ɫ\0");
            ImGui::SameLine();
            ImGuiColorEditFlags colorFlags = ImGuiColorEditFlags_HDR;
            ImGui::ColorEdit4(u8"##����ɫ", glm::value_ptr(color4), colorFlags);

            lightComponent.light->SetColor(glm::vec3(color4));

            //light's intensity
            float intensity = lightComponent.light->GetIntensity();
            ImGui::Text(u8"��   ǿ\0");
            ImGui::SameLine();
            ImGui::DragFloat("##Intensity", &intensity, 0.1, 0, 100);
            lightComponent.light->SetIntensity(intensity);

            auto PI = glm::pi<float>();

            if (lightComponent.type == LightType::Directional) 
            {
                auto light = dynamic_cast<DirectionalLight*>(lightComponent.light.get());
                auto& dir = light->GetDirection();
                ImGui::SetNextItemWidth(60);
                ImGui::Text(u8"��   ��\0");
                ImGui::SameLine();
                ImGui::DragFloat3(u8"##����", glm::value_ptr(dir), 0.01f, -2 * PI, 2 * PI, "%.3f");
                light->SetDirection(dir);
            }

            if (lightComponent.type == LightType::Point)
            {
                auto light = dynamic_cast<PointLight*>(lightComponent.light.get());
                auto& position = transformComponent.Translation;
                float linear = light->GetLinear();
                float quadratic = light->GetQuadratic();
                ImGui::SliderFloat(u8"һ��˥��ϵ��", &linear, 0.0f, 1.0f);
                light->SetLinear(linear);
                ImGui::SliderFloat(u8"����˥��ϵ��", &quadratic, 0.0f, 1.0f);
                light->SetQuadratic(quadratic);
            }

            if (lightComponent.type == LightType::Spot)
            {
                auto light = dynamic_cast<SpotLight*>(lightComponent.light.get());
                auto& spotDirection = transformComponent.Rotation;
                float iCut = light->GetInnerCutOff();
                float oCut = light->GetOuterCutOff();
                float linear = light->GetLinear();
                float quadratic = light->GetQuadratic();
                ImGui::SliderFloat(u8"�ھ�", &iCut, 0.0f, light->GetOuterCutOff() - 0.01f, "%.3f");
                ImGui::SliderFloat(u8"�⾶", &oCut, 0.0f, 180.0f, "%.3f");
                ImGui::SliderFloat(u8"һ��ϵ��", &linear, 0.0f, 1.0f);
                ImGui::SliderFloat(u8"����ϵ��", &quadratic, 0.0f, 1.0f);
                light->SetCutOff(iCut, oCut);
                light->SetLinear(linear);
                light->SetQuadratic(quadratic);
            }

            ImGui::TreePop();

            if (LightRemove) {
                entity.RemoveComponent<LightComponent>();
                LightRemove = false;
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