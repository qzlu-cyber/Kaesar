#include "krpch.h"
#include "SceneSerializer.h"

#include "Kaesar/Scene/Entity.h"
#include "Kaesar/Scene/Component.h"
#include "Kaesar/Scene/Scene.h"

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

    YAML::Emitter& operator<<(YAML::Emitter& out, const std::unordered_map<uint32_t, std::shared_ptr<Texture2D>>& textures)
    {
        for (auto&& [binding, texture] : textures)
        {
            std::string path = "";
            if (texture)
            {
                path = texture->GetPath();
            }
            out << YAML::Flow;
            out << YAML::BeginMap;
            out << YAML::Key << "binding" << YAML::Value << binding;
            out << YAML::Key << "path" << YAML::Value << path << YAML::EndMap;
        }

        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const Material::CBuffer& cbuffer)
    {
        out << YAML::Flow << YAML::BeginMap;
        out << YAML::Key << "Tiling" << YAML::Value << cbuffer.tiling << YAML::EndMap;

        out << YAML::Flow << YAML::BeginMap;
        out << YAML::Key << "Use Albedo" << YAML::Value << cbuffer.HasAlbedoMap;
        out << YAML::Key << "Albedo" << YAML::Value << cbuffer.material.color << YAML::EndMap;

        out << YAML::Flow << YAML::BeginMap;
        out << YAML::Key << "Use MetallicMap" << YAML::Value << cbuffer.HasMetallicMap;
        out << YAML::Key << "Metallic Factor" << YAML::Value << cbuffer.material.MetallicFactor << YAML::EndMap;

        out << YAML::Flow << YAML::BeginMap;
        out << YAML::Key << "Use NormalMap" << YAML::Value << cbuffer.HasNormalMap << YAML::EndMap;

        out << YAML::Flow << YAML::BeginMap;
        out << YAML::Key << "Use RoughnessMap" << YAML::Value << cbuffer.HasRoughnessMap;
        out << YAML::Key << "Roughness Factor" << YAML::Value << cbuffer.material.RoughnessFactor << YAML::EndMap;

        out << YAML::Flow << YAML::BeginMap;
        out << YAML::Key << "Use AOMap" << YAML::Value << cbuffer.HasAOMap;
        out << YAML::Key << "AO" << YAML::Value << cbuffer.material.AO << YAML::EndMap;

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

        if (entity.HasComponent<MaterialComponent>())
        {
            out << YAML::Key << "MaterialComponent";
            out << YAML::BeginMap; // MaterialComponent Begin

            auto& materialComponent = entity.GetComponent<MaterialComponent>();

            auto shader = materialComponent.material.GetShader();
            out << YAML::Key << "Shader" << YAML::Value << shader->GetName();

            out << YAML::Key << "Textures" << YAML::Value << YAML::BeginSeq;
            out << materialComponent.material.GetTextures();
            out << YAML::EndSeq;

            out << YAML::Key << "Constants" << YAML::Value << YAML::BeginSeq;
            out << materialComponent.material.GetCBuffer();
            out << YAML::EndSeq;

            out << YAML::EndMap;
        }

        if (entity.HasComponent<LightComponent>())
        {
            out << YAML::Key << "LightComponent";
            out << YAML::BeginMap; // LightComponent Begin

            auto& lightComponent = entity.GetComponent<LightComponent>();
            out << YAML::Key << "Type" << YAML::Value << int(lightComponent.type);
            out << YAML::Key << "Intensity" << YAML::Value << lightComponent.light->GetIntensity();

            out << YAML::Key << "Color" << YAML::Value << lightComponent.light->GetColor();

            switch (lightComponent.type)
            {
                case LightType::Directional:
                    out << YAML::Key << "Direction" << YAML::Value << dynamic_cast<DirectionalLight*>(lightComponent.light.get())->GetDirection();
                    break;
                case LightType::Point:
                    out << YAML::Key << "Position" << YAML::Value << dynamic_cast<PointLight*>(lightComponent.light.get())->GetPosition();
                    out << YAML::Key << "Linear" << YAML::Value << dynamic_cast<PointLight*>(lightComponent.light.get())->GetLinear();
                    out << YAML::Key << "Quadratic" << YAML::Value << dynamic_cast<PointLight*>(lightComponent.light.get())->GetQuadratic();
                    break;
                case LightType::Spot:
                    out << YAML::Key << "Position" << YAML::Value << dynamic_cast<SpotLight*>(lightComponent.light.get())->GetPosition();
                    out << YAML::Key << "Direction" << YAML::Value << dynamic_cast<SpotLight*>(lightComponent.light.get())->GetDirection();
                    out << YAML::Key << "Linear" << YAML::Value << dynamic_cast<SpotLight*>(lightComponent.light.get())->GetLinear();
                    out << YAML::Key << "Quadratic" << YAML::Value << dynamic_cast<SpotLight*>(lightComponent.light.get())->GetQuadratic();
                    out << YAML::Key << "InnerCutOff" << YAML::Value << dynamic_cast<SpotLight*>(lightComponent.light.get())->GetInnerCutOff();
                    out << YAML::Key << "OuterCutOff" << YAML::Value << dynamic_cast<SpotLight*>(lightComponent.light.get())->GetOuterCutOff();
                    break;
                default:
                    break;
            }

            out << YAML::EndMap; // LightComponent End
        }

        out << YAML::EndMap; // Entity End
    }

    SceneSerializer::SceneSerializer(const std::shared_ptr<Scene>& scene)
        : m_Scene(scene)
    {
        m_Shaders = SceneRenderer::GetShaderLibrary(); // 获取到所有的 shader
    }

    void SceneSerializer::Serializer(const std::string& filepath)
    {
        YAML::Emitter out;

        auto nameWithPost = filepath.substr(filepath.find_last_of("\\") + 1);
        auto name = nameWithPost.substr(0, nameWithPost.find("."));

        out << YAML::BeginMap;

        out << YAML::Key << "Scene" << YAML::Value << name;

        // environment
        out << YAML::Key << "Environment path" << YAML::Value << m_Scene->m_EnvironmentPath;

        // Camera
        out << YAML::Key << "Camera" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "FOV" << YAML::Value << m_Scene->m_Camera->GetFOV();
        out << YAML::Key << "Near" << YAML::Value << m_Scene->m_Camera->GetNear();
        out << YAML::Key << "Far" << YAML::Value << m_Scene->m_Camera->GetFar();
        out << YAML::Key << "Yaw" << YAML::Value << m_Scene->m_Camera->GetYaw();
        out << YAML::Key << "Pitch" << YAML::Value << m_Scene->m_Camera->GetPitch();
        out << YAML::Key << "Position" << YAML::Value << m_Scene->m_Camera->GetPosition();
        out << YAML::Key << "FocalPoint" << YAML::Value << m_Scene->m_Camera->GetFocalPoint();
        out << YAML::Key << "Distance" << YAML::Value << m_Scene->m_Camera->GetDistance();
        out << YAML::EndMap;

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
        YAML::Node data = YAML::LoadFile(filepath);

        if (!data["Scene"])
            return false;

        std::string sceneName = data["Scene"].as<std::string>();
        m_Scene->m_Name = sceneName;
        KR_CORE_TRACE("读取场景 '{0}'", sceneName);

        if (data["Environment path"])
        {
            auto hdriPath = data["Environment path"].as<std::string>();
            m_Scene->m_EnvironmentPath = hdriPath;
        }

        auto camera = data["Camera"];
        auto fov = camera["FOV"].as<float>();
        auto nearClip = camera["Near"].as<float>();
        auto farClip = camera["Far"].as<float>();
        auto yaw = camera["Yaw"].as<float>();
        auto pitch = camera["Pitch"].as<float>();
        auto focalPoint = camera["FocalPoint"].as<glm::vec3>();
        auto distance = camera["Distance"].as<float>();

        m_Scene->m_Camera->SetFov(fov);
        m_Scene->m_Camera->SetNearClip(nearClip);
        m_Scene->m_Camera->SetFarClip(farClip);
        m_Scene->m_Camera->SetYawPitch(yaw, pitch);
        m_Scene->m_Camera->SetFocalPoint(focalPoint);
        m_Scene->m_Camera->SetDistance(distance);

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

                auto meshComponent = entity["MeshComponent"];
                if (meshComponent)
                {
                    auto dir = std::filesystem::current_path();
                    auto& mc = deserializedEntity.AddComponent<MeshComponent>();
                    mc.path = meshComponent["Path"].as<std::string>();
                    if (mc.path.find("\\") == 0) {
                        mc.path = dir.string() + mc.path;
                    }
                    mc.model = Model(mc.path);
                }

                auto& materialComponent = entity["MaterialComponent"];
                if (materialComponent)
                {
                    auto dir = std::filesystem::current_path();
                    auto shaderName = materialComponent["Shader"].as<std::string>();
                    auto shader = m_Shaders.Get(shaderName);

                    auto material = Material::Create(shader);
                    auto& materialTextures = material->GetTextures();

                    auto textures = materialComponent["Textures"];
                    if (textures)
                    {
                        for (auto texture : textures)
                        {
                            auto binding = texture["binding"].as<uint32_t>();
                            auto texturePath = texture["path"].as<std::string>();
                            if (!texturePath.empty()) 
                            {
                                materialTextures[binding] = Texture2D::Create(texturePath, 0);
                            }
                        }
                    }

                    auto cbuffer = materialComponent["Constants"];
                    if (cbuffer)
                    {
                        material->Set("tiling", cbuffer[0]["Tiling"].as<float>());

                        material->Set("HasAlbedoMap", cbuffer[1]["Use Albedo"].as<int>());
                        material->Set("push.material.color", cbuffer[1]["Albedo"].as<glm::vec4>());

                        material->Set("HasNormalMap", cbuffer[2]["Use MetallicMap"].as<int>());
                        material->Set("push.material.MetallicFactor", cbuffer[2]["Metallic Factor"].as<float>());

                        material->Set("HasRoughnessMap", cbuffer[3]["Use NormalMap"].as<int>());

                        material->Set("HasMetallicMap", cbuffer[4]["Use RoughnessMap"].as<int>());
                        material->Set("push.material.RoughnessFactor", cbuffer[4]["Roughness Factor"].as<float>());

                        material->Set("HasAOMap", cbuffer[5]["Use AOMap"].as<int>());
                        material->Set("push.material.AO", cbuffer[5]["AO"].as<float>());
                    }

                    deserializedEntity.AddComponent<MaterialComponent>(material);
                }

                auto lightComponent = entity["LightComponent"];
                if (lightComponent)
                {
                    auto& lc = deserializedEntity.AddComponent<LightComponent>();
                    auto strType = lightComponent["Type"].as<int>();
                    auto intensity = lightComponent["Intensity"].as<float>();
                    auto color = lightComponent["Color"].as<glm::vec3>();

                    if (strType == 0)
                    {
                        lc.type = LightType::Directional;
                        auto direction = lightComponent["Direction"].as<glm::vec3>();
                        lc.light = std::make_shared<DirectionalLight>(direction, color, intensity);
                    }
                    if (strType == 1)
                    {
                        lc.type = LightType::Point;
                        auto position = transformComponent["Translation"].as<glm::vec3>();
                        auto linear = lightComponent["Linear"].as<float>();
                        auto quadratic = lightComponent["Quadratic"].as<float>();
                        lc.light = std::make_shared<PointLight>(position, color, intensity, linear, quadratic);
                    }
                    if (strType == 2)
                    {
                        lc.type = LightType::Spot;
                        auto direction = transformComponent["Rotation"].as<glm::vec3>();
                        auto position = transformComponent["Translation"].as<glm::vec3>();
                        auto linear = lightComponent["Linear"].as<float>();
                        auto quadratic = lightComponent["Quadratic"].as<float>();
                        auto innerCutOff = lightComponent["InnerCutOff"].as<float>();
                        auto outerCutOff = lightComponent["OuterCutOff"].as<float>();
                        lc.light = std::make_shared<SpotLight>(position, direction, color, 
                                                               intensity, linear, quadratic, innerCutOff, outerCutOff);
                    }
                }
            }
        }

        return true;
    }
}