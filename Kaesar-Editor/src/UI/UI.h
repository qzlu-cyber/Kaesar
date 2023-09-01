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

        /// <summary>
        /// 一个函数模板，允许在一个统一的方式下绘制不同类型的组件的用户界面
        /// </summary>
        /// <typeparam name="T">组件类型</typeparam>
        /// <param name="name">组件名称</param>
        /// <param name="entity">要绘制的当前实体</param>
        /// <param name="removable">此组件是否可移除</param>
        /// <param name="removed">组件是否被移除</param>
        /// <returns>函数返回一个布尔值，表示树节点是否展开</returns>
        template<typename T>
        static bool DrawComponent(const std::string& name, Entity entity, bool removable, bool* removed)
        {
            const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed
                | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap
                | ImGuiTreeNodeFlags_FramePadding;

            // 判断给定实体是否拥有指定类型的组件。如果是，那么就继续绘制这个组件
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
                        if (ImGui::MenuItem(u8"删除组件"))
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