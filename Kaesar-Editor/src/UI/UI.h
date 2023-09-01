#pragma once

#include "Kaesar/Scene/Entity.h"
#include "Kaesar/ImGui/IconsFontAwesome5.h"

#include <string>
#include <codecvt>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

namespace Kaesar {
	class UI
	{
	public:
		static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
		static bool SliderFloat(const std::string& name, float* value, float min = 0.0f, float max = 1.0f);
		static bool DragFloat(const std::string& name, float* value, float speed = 0.1f, float min = 0.0f, float max = 100.0f);		
	
        static std::string DrawIconFont(const char* name, const char* icon);

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
                ImGui::PushID(name.c_str());

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

                ImGui::PopID();

                if (removeComponent)
                    *removed = true;

                return open;
            }
            return false;
        }
	
    private:
        static std::string TBS(std::string& str);
    };
}