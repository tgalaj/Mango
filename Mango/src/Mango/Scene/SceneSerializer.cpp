#include "mgpch.h"
#include "SceneSerializer.h"
#include "Entity.h"

#include <yaml-cpp/yaml.h>

#include <fstream>

namespace YAML {

    template<>
    struct convert<glm::vec2>
    {
        static Node encode(const glm::vec2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };

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

namespace mango
{
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::quat& q)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << q.w << q.x << q.y << q.z << YAML::EndSeq;
        return out;
    }

    static void serializeEntity(YAML::Emitter& out, Entity entity)
    {
        out << YAML::BeginMap; // Entity
        out << YAML::Key << "Entity" << YAML::Value << 9823748; // TODO: UUID for entity

        if (entity.hasComponent<TagComponent>())
        {
            out << YAML::Key << "TagComponent";
            out << YAML::BeginMap;
            {
                auto& tag = entity.getComponent<TagComponent>().tag;
                out << YAML::Key << "Tag" << YAML::Value << tag;
            }
            out << YAML::EndMap;
        }

        if (entity.hasComponent<TransformComponent>())
        {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap;
            {
                auto& tc = entity.getComponent<TransformComponent>();
                out << YAML::Key << "Position"    << YAML::Value << tc.getPosition();
                out << YAML::Key << "Rotation"    << YAML::Value << glm::degrees(tc.getRotation());
                out << YAML::Key << "Orientation" << YAML::Value << tc.getOrientation();
                out << YAML::Key << "Scale"       << YAML::Value << tc.getScale();
            }
            out << YAML::EndMap;
        }

        if (entity.hasComponent<CameraComponent>())
        {
            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap;
            {
                auto& camera = entity.getComponent<CameraComponent>();
                out << YAML::Key << "ProjectionType"   << YAML::Value << (int)camera.getProjectionType();
                out << YAML::Key << "PerspectiveFOV"   << YAML::Value << camera.getPerspectiveVerticalFieldOfView();
                out << YAML::Key << "PerspectiveNear"  << YAML::Value << camera.getPerspectiveNearClip();
                out << YAML::Key << "PerspectiveFar"   << YAML::Value << camera.getPerspectiveFarClip();
                out << YAML::Key << "OrthographicSize" << YAML::Value << camera.getOrthographicSize();
                out << YAML::Key << "OrthographicNear" << YAML::Value << camera.getOrthographicNearClip();
                out << YAML::Key << "OrthographicFar"  << YAML::Value << camera.getOrthographicFarClip();
                out << YAML::Key << "IsPrimary"        << YAML::Value << camera.isPrimary();
            }
            out << YAML::EndMap;
        }

        out << YAML::EndMap; // Entity
    }

    bool SceneSerializer::serialize(const std::shared_ptr<Scene>& scene, const std::filesystem::path& outFilepath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << scene->getName();
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

        scene->m_registry.each([&](auto entityID)
        {
            Entity entity = { entityID, scene.get() };
            
            if (!entity) return false;

            serializeEntity(out, entity);
        });

        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(outFilepath);
        if (fout.is_open())
        {
            fout << out.c_str();
            return true;
        }

        return false;
    }

    bool SceneSerializer::serializeBinary(const std::shared_ptr<Scene>& scene, const std::filesystem::path& outFilepath)
    {
        MG_CORE_ASSERT_MSG(false, "Not implemented!");
        return true;
    }

    std::shared_ptr<Scene> SceneSerializer::deserialize(const std::filesystem::path& inFilepath)
    {
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(inFilepath.string());
        }
        catch (YAML::ParserException e)
        {
            MG_CORE_ERROR("Failed to load .mango file '{}'\n    {}", inFilepath.string(), e.what());
            return nullptr;
        }

        // Scene is a mandatory node
        if (!data["Scene"])
            return nullptr;

        std::string sceneName = data["Scene"].as<std::string>();
        auto scene = std::make_shared<Scene>(sceneName);

        MG_CORE_TRACE("Deserializing scene '{}'", sceneName);

        auto entities = data["Entities"];
        if (entities)
        {
            for (auto entity : entities)
            {
                uint64_t uuid = entity["Entity"].as<uint64_t>(); // TODO: UUID component

                std::string entityName;
                auto tagComponent = entity["TagComponent"];
                if (tagComponent)
                {
                    entityName = tagComponent["Tag"].as<std::string>();
                }
                MG_CORE_TRACE("\tDeserializing entity with ID = {}, name = {}", uuid, entityName);

                Entity deserializedEntity = scene->createEntity(entityName);

                auto transformComponent = entity["TransformComponent"];
                if (transformComponent)
                {
                    // Entities always have transforms
                    auto& tc = deserializedEntity.getComponent<TransformComponent>();
                    tc.setPosition(transformComponent["Position"].as<glm::vec3>());
                    tc.setRotation(transformComponent["Rotation"].as<glm::vec3>());
                    tc.setScale   (transformComponent["Scale"].as<glm::vec3>());
                }

                auto cameraComponent = entity["CameraComponent"];
                if (cameraComponent)
                {
                    auto& camera = deserializedEntity.addComponent<CameraComponent>();
                    camera.setProjectionType                ((CameraComponent::ProjectionType)cameraComponent["ProjectionType"].as<int>());
                    camera.setPerspectiveVerticalFieldOfView(glm::degrees(cameraComponent["PerspectiveFOV"].as<float>()));
                    camera.setPerspectiveNearClip           (cameraComponent["PerspectiveNear"].as<float>());
                    camera.setPerspectiveFarClip            (cameraComponent["PerspectiveFar"].as<float>());
                    camera.setOrthographicSize              (cameraComponent["OrthographicSize"].as<float>());
                    camera.setOrthographicNearClip          (cameraComponent["OrthographicNear"].as<float>());
                    camera.setOrthographicFarClip           (cameraComponent["OrthographicFar"].as<float>());
                    
                    if (cameraComponent["IsPrimary"].as<bool>())
                        camera.setPrimary();
                }
            }
        }

        return scene;
    }

    std::shared_ptr<Scene> SceneSerializer::deserializeBinary(const std::filesystem::path& inFilepath)
    {
        MG_CORE_ASSERT_MSG(false, "Not implemented!");
        return nullptr;
    }
}