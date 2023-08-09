#include "krpch.h"
#include "SceneSerializer.h"

#include "Kaesar/Scene/Entity.h"
#include "Kaesar/Scene/Component.h"

#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>

namespace YAML {
    template<>
    struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.SetStyle(EmitterStyle::Flow);

            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();

            return true;
        }
    };

    template<>
    struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);

            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();

            return true;
        }
    };
}

namespace Kaesar {
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& vec)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;

        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& vec)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << vec.x << vec.y << vec.z << vec.w << YAML::EndSeq;

        return out;
    }

    void SerializeEntity(YAML::Emitter& out, Entity entity)
    {
        out << YAML::BeginMap; // Entity Begiin
        out << YAML::Key << "Entity" << YAML::Value << (uint32_t)entity;

        if (entity.HasComponent<TagComponent>())
        {
            out << YAML::Key << "TagComponent";
            out << YAML::BeginMap; // Tag Begin

            auto& tag = entity.GetComponent<TagComponent>().Tag;
            out << YAML::Key << "Tag" << YAML::Value << tag;

            out << YAML::EndMap; // Tag End
        }

        if (entity.HasComponent<TransformComponent>())
        {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap; // Trans Begin

            auto& trans = entity.GetComponent<TransformComponent>();
            out << YAML::Key << "Translation" << YAML::Value << trans.Translation;
            out << YAML::Key << "Rotation" << YAML::Value << trans.Rotation;
            out << YAML::Key << "Scale" << YAML::Value << trans.Scale;

            out << YAML::EndMap; // Trans End
        }

        if (entity.HasComponent<CameraComponent>())
        {
            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap; // CameraComponent Begin

            auto& cameraComponent = entity.GetComponent<CameraComponent>();
            auto& camera = cameraComponent.Camera;

            out << YAML::Key << "Camera" << YAML::Value;
            out << YAML::BeginMap; // Camera Begin
            out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
            out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveFOV();
            out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
            out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
            out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
            out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
            out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
            out << YAML::EndMap; // Camera End

            out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
            out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

            out << YAML::EndMap; // CameraComponent End
        }

        if (entity.HasComponent<MeshComponent>())
        {
            out << YAML::Key << "MeshComponent";
            out << YAML::BeginMap; // MeshComponent Begin

            auto& meshComponent = entity.GetComponent<MeshComponent>();
            out << YAML::Key << "Path" << YAML::Value << meshComponent.path;

            out << YAML::EndMap; // MeshComponent End
        }

        out << YAML::EndMap; // Entity End
    }

    SceneSerializer::SceneSerializer(const std::shared_ptr<Scene>& scene)
        : m_Scene(scene)
    {}

    void SceneSerializer::Serializer(const std::string& filepath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;

        out << YAML::Key << "Scene" << YAML::Value << u8"未命名";
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
      
        for (auto& entity : m_Scene->m_Entities)
        {
            SerializeEntity(out, *entity);
        }

        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();
    }

    bool SceneSerializer::Deserializer(const std::string& filepath)
    {
        YAML::Node data = YAML::Load(filepath);

        if (!data["Scene"])
            return false;

        std::string sceneName = data["Scene"].as<std::string>();
        KR_CORE_TRACE("读取场景 '{0}'", sceneName);

        auto entities = data["Entities"];
        if (entities)
        {
            for (auto entity : entities)
            {
                uint64_t uuid = entity["Entity"].as<uint64_t>();

                std::string name;
                auto tagComponent = entity["TagComponent"];
                if (tagComponent)
                    name = tagComponent["Tag"].as<std::string>();

                KR_CORE_TRACE("读取到实体 ID = {0}, 名称 = {1}", uuid, name);

                Entity deserializedEntity = m_Scene->CreateEntity(name);

                auto transformComponent = entity["TransformComponent"];
                if (transformComponent)
                {
                    auto& trans = deserializedEntity.GetComponent<TransformComponent>();
                    trans.Translation = transformComponent["Translation"].as<glm::vec3>();
                    trans.Rotation = transformComponent["Rotation"].as<glm::vec3>();
                    trans.Scale = transformComponent["Scale"].as<glm::vec3>();
                }

                auto cameraComponent = entity["CameraComponent"];
                if (cameraComponent)
                {
                    auto& cc = deserializedEntity.AddComponent<CameraComponent>();
                    cc.Primary = cameraComponent["Primary"].as<bool>();
                    cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();

                    auto& camera = cc.Camera;
                    auto cameraProps = cameraComponent["Camera"];
                    camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>()); 
                    camera.SetPerspectiveFOV(cameraProps["PerspectiveFOV"].as<float>());
                    camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
                    camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());       
                    camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
                    camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
                    camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());
                }
            }
        }

        return true;
    }
}