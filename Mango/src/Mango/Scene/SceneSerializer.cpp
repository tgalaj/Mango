#include "mgpch.h"

#include "Entity.h"
#include "SceneSerializer.h"
#include "Mango/Core/AssetManager.h"

#include <yaml-cpp/yaml.h>
#include <fstream>
#include <unordered_set>

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
        out << YAML::Key << "Entity" << YAML::Value << entity.getUUID();

        if (entity.hasComponent<TagComponent>())
        {
            out << YAML::Key << "TagComponent";
            out << YAML::BeginMap;
            {
                auto& name = entity.getComponent<TagComponent>().name;
                out << YAML::Key << "Name" << YAML::Value << name;
            }
            out << YAML::EndMap;
        }

        if (entity.hasComponent<TransformComponent>())
        {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap;
            {
                auto& tc = entity.getComponent<TransformComponent>();
                out << YAML::Key << "Position"    << YAML::Value << tc.getLocalPosition();
                out << YAML::Key << "Rotation"    << YAML::Value << glm::degrees(tc.getLocalRotation());
                out << YAML::Key << "Orientation" << YAML::Value << tc.getLocalOrientation();
                out << YAML::Key << "Scale"       << YAML::Value << tc.getLocalScale();
                out << YAML::Key << "ParentID"    << YAML::Value << (tc.hasParent() ? tc.getParent().getUUID() : (UUID)0);
                out << YAML::Key << "Children"    << YAML::Value;
                out << YAML::BeginSeq;
                {
                    for (auto child : tc.getChildren())
                    {
                        out << child.getUUID();
                    }
                }
                out << YAML::EndSeq;
            }
            out << YAML::EndMap;
        }

        if (entity.hasComponent<DirectionalLightComponent>())
        {
            out << YAML::Key << "DirectionalLightComponent";
            out << YAML::BeginMap;
            {
                auto& dlc = entity.getComponent<DirectionalLightComponent>();
                out << YAML::Key << "Color"        << YAML::Value << dlc.color;
                out << YAML::Key << "Intensity"    << YAML::Value << dlc.intensity;
                out << YAML::Key << "Size"         << YAML::Value << dlc.getSize();
                out << YAML::Key << "CastsShadows" << YAML::Value << dlc.getCastsShadows();
            }
            out << YAML::EndMap;
        }

        if (entity.hasComponent<PointLightComponent>())
        {
            out << YAML::Key << "PointLightComponent";
            out << YAML::BeginMap;
            {
                auto& plc = entity.getComponent<PointLightComponent>();
                out << YAML::Key << "Color"        << YAML::Value << plc.color;
                out << YAML::Key << "Intensity"    << YAML::Value << plc.intensity;
                out << YAML::Key << "Attenuation"  << YAML::Value;
                out << YAML::BeginMap;
                {
                    auto attenuation = plc.getAttenuation();
                    out << YAML::Key << "Constant"  << YAML::Value << attenuation.constant;
                    out << YAML::Key << "Linear"    << YAML::Value << attenuation.linear;
                    out << YAML::Key << "Quadratic" << YAML::Value << attenuation.quadratic;
                }
                out << YAML::EndMap;
                out << YAML::Key << "ShadowNearPlane" << YAML::Value << plc.getShadowNearPlane();
                out << YAML::Key << "ShadowFarPlane"  << YAML::Value << plc.getShadowFarPlane();
                out << YAML::Key << "CastsShadows"    << YAML::Value << plc.getCastsShadows();
            }
            out << YAML::EndMap;
        }

        if (entity.hasComponent<SpotLightComponent>())
        {
            out << YAML::Key << "SpotLightComponent";
            out << YAML::BeginMap;
            {
                auto& slc = entity.getComponent<SpotLightComponent>();
                out << YAML::Key << "Color"       << YAML::Value << slc.color;
                out << YAML::Key << "Intensity"   << YAML::Value << slc.intensity;
                out << YAML::Key << "CutOffAngle" << YAML::Value << glm::degrees(slc.getCutOffAngle());
                out << YAML::Key << "Attenuation" << YAML::Value;
                out << YAML::BeginMap;
                {
                    auto attenuation = slc.getAttenuation();
                    out << YAML::Key << "Constant"  << YAML::Value << attenuation.constant;
                    out << YAML::Key << "Linear"    << YAML::Value << attenuation.linear;
                    out << YAML::Key << "Quadratic" << YAML::Value << attenuation.quadratic;
                }
                out << YAML::EndMap;
                out << YAML::Key << "ShadowNearPlane" << YAML::Value << slc.getShadowNearPlane();
                out << YAML::Key << "ShadowFarPlane"  << YAML::Value << slc.getShadowFarPlane();
                out << YAML::Key << "CastsShadows"    << YAML::Value << slc.getCastsShadows();
            }
            out << YAML::EndMap;
        }

        if (entity.hasComponent<CameraComponent>())
        {
            auto projectionTypeToString = [](Camera::ProjectionType type) -> std::string
            {
                switch (type)
                {
                    case Camera::ProjectionType::Perspective:  return "Perspective";
                    case Camera::ProjectionType::Orthographic: return "Orthographic";
                }
                MG_CORE_ASSERT_MSG(false, "Unknown projection type");
                return {};
            };

            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap;
            {
                auto& cc     = entity.getComponent<CameraComponent>();
                auto& camera = cc.camera;

                out << YAML::Key << "ProjectionType"   << YAML::Value << projectionTypeToString(camera.getProjectionType());
                out << YAML::Key << "PerspectiveFOV"   << YAML::Value << glm::degrees(camera.getPerspectiveVerticalFieldOfView());
                out << YAML::Key << "PerspectiveNear"  << YAML::Value << camera.getPerspectiveNearClip();
                out << YAML::Key << "PerspectiveFar"   << YAML::Value << camera.getPerspectiveFarClip();
                out << YAML::Key << "OrthographicSize" << YAML::Value << camera.getOrthographicSize();
                out << YAML::Key << "OrthographicNear" << YAML::Value << camera.getOrthographicNearClip();
                out << YAML::Key << "OrthographicFar"  << YAML::Value << camera.getOrthographicFarClip();
                out << YAML::Key << "IsPrimary"        << YAML::Value << cc.isPrimary;
            }
            out << YAML::EndMap;
        }

        if (entity.hasComponent<StaticMeshComponent>())
        {
            out << YAML::Key << "StaticMeshComponent";
            out << YAML::BeginMap;
            {
                auto& smc = entity.getComponent<StaticMeshComponent>();
                out << YAML::Key << "Filename"  << YAML::Value << smc.mesh->getName();
                out << YAML::Key << "Materials" << YAML::Value;
                out << YAML::BeginMap;
                {
                    auto originalMaterials = smc.mesh->getMaterials();

                    if (!smc.materials.empty())
                    {
                        for (uint32_t i = 0; i < smc.materials.size(); ++i)
                        {
                            auto& material = smc.materials[i];

                            if (material != originalMaterials[i])
                            {
                                out << YAML::Key << i << YAML::Value << material->name;
                            }
                        }
                    }
                }
                out << YAML::EndMap;
            }
            out << YAML::EndMap;
        }

        if (entity.hasComponent<RigidBody3DComponent>())
        {
            auto motionTypeToString = [](RigidBody3DComponent::MotionType type) -> std::string
            {
                switch (type)
                {
                    case RigidBody3DComponent::MotionType::Static:    return "Static";
                    case RigidBody3DComponent::MotionType::Kinematic: return "Kinematic";
                    case RigidBody3DComponent::MotionType::Dynamic:   return "Dynamic";
                }
                MG_CORE_ASSERT_MSG(false, "Unknown motion type");
                return {};
            };

            out << YAML::Key << "RigidBody3DComponent";
            out << YAML::BeginMap;
            {
                auto& rb3d = entity.getComponent<RigidBody3DComponent>();
                out << YAML::Key << "MotionType"     << YAML::Value << motionTypeToString(rb3d.motionType);
                out << YAML::Key << "Friction"       << YAML::Value << rb3d.friction;
                out << YAML::Key << "Restitution"    << YAML::Value << rb3d.restitution;
                out << YAML::Key << "LinearDamping"  << YAML::Value << rb3d.linearDamping;
                out << YAML::Key << "AngularDamping" << YAML::Value << rb3d.angularDamping;
                out << YAML::Key << "Activated"      << YAML::Value << rb3d.isInitiallyActivated;
            }
            out << YAML::EndMap;
        }

        if (entity.hasComponent<BoxCollider3DComponent>())
        {
            out << YAML::Key << "BoxCollider3DComponent";
            out << YAML::BeginMap;
            {
                auto& bc3d = entity.getComponent<BoxCollider3DComponent>();
                out << YAML::Key << "Offset" << YAML::Value << bc3d.offset;
                out << YAML::Key << "Extent" << YAML::Value << bc3d.halfExtent;
            }
            out << YAML::EndMap;
        }

        if (entity.hasComponent<CapsuleColliderComponent>())
        {
            out << YAML::Key << "CapsuleColliderComponent";
            out << YAML::BeginMap;
            {
                auto& cc = entity.getComponent<CapsuleColliderComponent>();
                out << YAML::Key << "Offset"     << YAML::Value << cc.offset;
                out << YAML::Key << "HalfHeight" << YAML::Value << cc.halfHeight;
                out << YAML::Key << "Radius"     << YAML::Value << cc.radius;
            }
            out << YAML::EndMap;
        }

        if (entity.hasComponent<SphereColliderComponent>())
        {
            out << YAML::Key << "SphereColliderComponent";
            out << YAML::BeginMap;
            {
                auto& sc = entity.getComponent<SphereColliderComponent>();
                out << YAML::Key << "Offset" << YAML::Value << sc.offset;
                out << YAML::Key << "Radius" << YAML::Value << sc.radius;
            }
            out << YAML::EndMap;
        }

        out << YAML::EndMap; // Entity
    }
    
    bool SceneSerializer::serialize(const ref<Scene>& scene, const std::filesystem::path& outFilepath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene"     << YAML::Value << scene->getName();
        out << YAML::Key << "Materials" << YAML::Value << YAML::BeginSeq;

        // NOTE(TG): serialize materials first (this will change when we have proper asset system - materials saved in files)
        auto renderQueueToString = [](Material::RenderQueue queue) -> std::string
        {
            switch (queue)
            {
                case Material::RenderQueue::RQ_OPAQUE:                 return "Opaque";
                case Material::RenderQueue::RQ_TRANSPARENT:            return "Transparent";
                case Material::RenderQueue::RQ_ENVIRO_MAPPING_STATIC:  return "StaticEnvMapping";
                case Material::RenderQueue::RQ_ENVIRO_MAPPING_DYNAMIC: return "DynamicEnvMapping";
            }
            MG_CORE_ASSERT_MSG(false, "Unknown render queue type");
            return {};
        };

        auto materialTextureTypeToString = [](Material::TextureType type) -> std::string
        {
            switch (type)
            {
                case Material::TextureType::DIFFUSE:      return "Diffuse";
                case Material::TextureType::SPECULAR:     return "Specular";
                case Material::TextureType::NORMAL:       return "Normal";
                case Material::TextureType::EMISSION:     return "Emission";
                case Material::TextureType::DISPLACEMENT: return "Displacement";
            }
            MG_CORE_ASSERT_MSG(false, "Unknown texture type");
            return {};
        };

        auto materialBlendModeToString = [](Material::BlendMode mode) -> std::string
        {
            switch (mode)
            {
                case Material::BlendMode::NONE:   return "None";
                case Material::BlendMode::ALPHA : return "Alpha";
            }
            MG_CORE_ASSERT_MSG(false, "Unknown blend mode");
            return {};
        };

        std::unordered_set<std::string> alreadySerializedMaterials;
        auto view = scene->getEntitiesWithComponent<StaticMeshComponent>();
        for (auto entityID : view)
        {
            auto& smc = view.get<StaticMeshComponent>(entityID);

            if (!std::filesystem::path(smc.mesh->getName()).has_extension()) // check if mesh name has an extension, if yes, then don't store the materials
            {
                auto originalMaterials = smc.mesh->getMaterials();

                if (!smc.materials.empty())
                {
                    for (uint32_t i = 0; i < smc.materials.size(); ++i)
                    {
                        auto& material = smc.materials[i];

                        if (material != originalMaterials[i] && !alreadySerializedMaterials.contains(material->name))
                        {
                            alreadySerializedMaterials.insert(material->name);

                            out << YAML::BeginMap;
                            {
                                out << YAML::Key << "Name" << YAML::Value << material->name;

                                out << YAML::Key << "Textures" << YAML::Value;
                                out << YAML::BeginMap;
                                {
                                    for (auto& [type, texture] : material->getTextureMap())
                                    {
                                        out << YAML::Key << materialTextureTypeToString(type) << YAML::Value << texture->getFilename();
                                    }
                                }
                                out << YAML::EndMap;

                                out << YAML::Key << "Vec3Map" << YAML::Value;
                                out << YAML::BeginMap;
                                {
                                    for (auto& [name, value] : material->getVec3Map())
                                    {
                                        out << YAML::Key << name << YAML::Value << value;
                                    }
                                }
                                out << YAML::EndMap;

                                out << YAML::Key << "FloatMap" << YAML::Value;
                                out << YAML::BeginMap;
                                {
                                    for (auto& [name, value] : material->getFloatMap())
                                    {
                                        out << YAML::Key << name << YAML::Value << value;
                                    }
                                }
                                out << YAML::EndMap;

                                out << YAML::Key << "BoolMap" << YAML::Value;
                                out << YAML::BeginMap;
                                {
                                    for (auto& [name, value] : material->getBoolMap())
                                    {
                                        out << YAML::Key << name << YAML::Value << value;
                                    }
                                }
                                out << YAML::EndMap;

                                out << YAML::Key << "BlendMode"   << YAML::Value << materialBlendModeToString(material->getBlendMode());
                                out << YAML::Key << "RenderQueue" << YAML::Value << renderQueueToString(material->getRenderQueue());
                            }
                            out << YAML::EndMap;
                        }
                    }
                }
            }
        }
        out << YAML::EndSeq;

        // Serialize entities now
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

        for (auto [entityID] : scene->m_registry.storage<entt::entity>().each())
        {
            Entity entity = { entityID, scene.get() };
            
            if (!entity) return false;

            serializeEntity(out, entity);
        }

        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(outFilepath);
        if (fout.is_open())
        {
            fout << out.c_str();
            return true;
        }
        else
        {
            MG_CORE_ERROR("Can't save file {}", outFilepath);
        }

        return false;
    }

    ref<Scene> SceneSerializer::deserialize(const std::filesystem::path& inFilepath)
    {
        YAML::Node data;
        try
        {
            data = YAML::LoadFile(inFilepath.string());
        }
        catch (YAML::ParserException e)
        {
            MG_CORE_ERROR("Failed to load scene file '{}'\n    {}", inFilepath.string(), e.what());
            return nullptr;
        }

        // Scene is a mandatory node
        if (!data["Scene"])
            return nullptr;

        std::string sceneName = data["Scene"].as<std::string>();
        auto scene = createRef<Scene>(sceneName);

        MG_CORE_TRACE("Deserializing scene '{}'", sceneName);

        // Deserialize materials first
        // NOTE(TG): when proper asset manager is implemented, this should be changed
        MG_CORE_TRACE("Deserializing materials");

        auto stringToRenderQueue = [](const std::string& s) -> Material::RenderQueue
        {
            if (s == "Opaque")            return Material::RenderQueue::RQ_OPAQUE;
            if (s == "Transparent")       return Material::RenderQueue::RQ_TRANSPARENT;
            if (s == "StaticEnvMapping")  return Material::RenderQueue::RQ_ENVIRO_MAPPING_STATIC;
            if (s == "DynamicEnvMapping") return Material::RenderQueue::RQ_ENVIRO_MAPPING_DYNAMIC;

            MG_CORE_ASSERT_MSG(false, "Unknown render queue");
            return Material::RenderQueue::RQ_OPAQUE;
        };

        auto stringToMaterialTextureType = [](const std::string& s) -> Material::TextureType
        {
            if (s == "Diffuse")      return Material::TextureType::DIFFUSE;
            if (s == "Specular")     return Material::TextureType::SPECULAR;
            if (s == "Normal")       return Material::TextureType::NORMAL;
            if (s == "Emission")     return Material::TextureType::EMISSION;
            if (s == "Displacement") return Material::TextureType::DISPLACEMENT;

            MG_CORE_ASSERT_MSG(false, "Unknown texture type");
            return Material::TextureType::DIFFUSE;
        };

        auto stringToMaterialBlendMode = [](const std::string& s) -> Material::BlendMode
        {
            if (s == "None")  return Material::BlendMode::NONE;
            if (s == "Alpha") return Material::BlendMode::ALPHA;

            MG_CORE_ASSERT_MSG(false, "Unknown blend mode");
            return Material::BlendMode::NONE;
        };

        auto materials = data["Materials"];
        if (materials)
        {
            for (auto material : materials)
            {
                auto materialName = material["Name"].as<std::string>();
                
                MG_CORE_TRACE("\tDeserializing material {}", materialName);
                
                auto mangoMaterial = AssetManagerOld::createMaterial(materialName);

                auto blendModeString = material["BlendMode"].as<std::string>();
                auto blendMode       = stringToMaterialBlendMode(blendModeString);
                mangoMaterial->setBlendMode(blendMode);

                auto renderQueueString = material["RenderQueue"].as<std::string>();
                auto renderQueue       = stringToRenderQueue(renderQueueString);
                mangoMaterial->setRenderQueue(renderQueue);

                auto textureMap = material["Textures"];
                if (textureMap)
                {
                    for (auto it = textureMap.begin(); it != textureMap.end(); ++it)
                    {
                        auto textureFilename = it->second.as<std::string>();

                        if (!textureFilename.empty())
                        {
                            auto textureType = stringToMaterialTextureType(it->first.as<std::string>());
                            
                            // TOOD: this is the legacy code, now we should read AssetHandle and store this in the material
                            //auto texture     = AssetManagerOld::createTexture2D(textureFilename, (textureType == Material::TextureType::DIFFUSE) ? true : false);

                            //mangoMaterial->addTexture(textureType, texture);
                        }
                    }
                }

                auto vec3Map = material["Vec3Map"];
                if (vec3Map)
                {
                    for (auto it = vec3Map.begin(); it != vec3Map.end(); ++it)
                    {
                        mangoMaterial->addVector3(it->first.as<std::string>(), it->second.as<glm::vec3>());
                    }
                }

                auto floatMap = material["FloatMap"];
                if (floatMap)
                {
                    for (auto it = floatMap.begin(); it != floatMap.end(); ++it)
                    {
                        mangoMaterial->addFloat(it->first.as<std::string>(), it->second.as<float>());
                    }
                }

                auto boolMap = material["BoolMap"];
                if (boolMap)
                {
                    for (auto it = boolMap.begin(); it != boolMap.end(); ++it)
                    {
                        mangoMaterial->addBool(it->first.as<std::string>(), it->second.as<bool>());
                    }
                }
            }
        }

        auto entities = data["Entities"];
        if (entities)
        {
            MG_CORE_TRACE("Deserializing entities...");
            for (auto entity : entities)
            {
                uint64_t uuid = entity["Entity"].as<uint64_t>();

                std::string entityName;
                auto tagComponent = entity["TagComponent"];
                if (tagComponent)
                {
                    entityName = tagComponent["Name"].as<std::string>();
                }
                MG_CORE_TRACE("\tDeserializing entity with ID = {}, name = {}", uuid, entityName);

                Entity deserializedEntity = scene->createEntityWithUUID(uuid, entityName);

                auto transformComponent = entity["TransformComponent"];
                if (transformComponent)
                {
                    // Entities always have transforms
                    auto& tc = deserializedEntity.getComponent<TransformComponent>();
                    tc.setLocalPosition(transformComponent["Position"].as<glm::vec3>());
                    tc.setLocalRotation(glm::radians(transformComponent["Rotation"].as<glm::vec3>()));
                    tc.setLocalScale   (transformComponent["Scale"].as<glm::vec3>());
                }

                auto directionalLightComponent = entity["DirectionalLightComponent"];
                if (directionalLightComponent)
                {
                    auto& dlc           = deserializedEntity.addComponent<DirectionalLightComponent>();
                          dlc.color     = directionalLightComponent["Color"].as<glm::vec3>();
                          dlc.intensity = directionalLightComponent["Intensity"].as<float>();
                          
                          dlc.setSize        (directionalLightComponent["Size"].as<float>());
                          dlc.setCastsShadows(directionalLightComponent["CastsShadows"].as<bool>());
                }

                auto pointLightComponent = entity["PointLightComponent"];
                if (pointLightComponent)
                {
                    auto& plc           = deserializedEntity.addComponent<PointLightComponent>();
                          plc.color     = pointLightComponent["Color"].as<glm::vec3>();
                          plc.intensity = pointLightComponent["Intensity"].as<float>();
                    
                    auto  attenuation = pointLightComponent["Attenuation"];
                    float constant    = attenuation["Constant"].as<float>();
                    float linear      = attenuation["Linear"].as<float>();
                    float quadratic   = attenuation["Quadratic"].as<float>();
                    
                    plc.setAttenuation(constant, linear, quadratic);
                    plc.setShadowNearPlane(pointLightComponent["ShadowNearPlane"].as<float>());
                    plc.setShadowFarPlane(pointLightComponent["ShadowFarPlane"].as<float>());
                    plc.setCastsShadows(pointLightComponent["CastsShadows"].as<bool>());
                }

                auto spotLightComponent = entity["SpotLightComponent"];
                if (spotLightComponent)
                {
                    auto& slc           = deserializedEntity.addComponent<SpotLightComponent>();
                          slc.color     = spotLightComponent["Color"].as<glm::vec3>();
                          slc.intensity = spotLightComponent["Intensity"].as<float>();
                          slc.setCutOffAngle(spotLightComponent["CutOffAngle"].as<float>());
                    
                    auto  attenuation = spotLightComponent["Attenuation"];
                    float constant    = attenuation["Constant"].as<float>();
                    float linear      = attenuation["Linear"].as<float>();
                    float quadratic   = attenuation["Quadratic"].as<float>();
                    
                    slc.setAttenuation(constant, linear, quadratic);
                    slc.setShadowNearPlane(spotLightComponent["ShadowNearPlane"].as<float>());
                    slc.setShadowFarPlane(spotLightComponent["ShadowFarPlane"].as<float>());
                    slc.setCastsShadows(spotLightComponent["CastsShadows"].as<bool>());
                }

                auto cameraComponent = entity["CameraComponent"];
                if (cameraComponent)
                {
                    auto stringToProjectionType = [](const std::string& s) -> Camera::ProjectionType
                    {
                        if (s == "Perspective")  return Camera::ProjectionType::Perspective;
                        if (s == "Orthographic") return Camera::ProjectionType::Orthographic;

                        MG_CORE_ASSERT_MSG(false, "Unknown projection type");
                        return Camera::ProjectionType::Perspective;
                    };

                    auto& cc     = deserializedEntity.addComponent<CameraComponent>();
                    auto& camera = cc.camera;

                    cc.isPrimary = cameraComponent["IsPrimary"].as<bool>();
                    camera.setProjectionType                (stringToProjectionType(cameraComponent["ProjectionType"].as<std::string>()));
                    camera.setPerspectiveVerticalFieldOfView(glm::radians(cameraComponent["PerspectiveFOV"].as<float>()));
                    camera.setPerspectiveNearClip           (cameraComponent["PerspectiveNear"].as<float>());
                    camera.setPerspectiveFarClip            (cameraComponent["PerspectiveFar"].as<float>());
                    camera.setOrthographicSize              (cameraComponent["OrthographicSize"].as<float>());
                    camera.setOrthographicNearClip          (cameraComponent["OrthographicNear"].as<float>());
                    camera.setOrthographicFarClip           (cameraComponent["OrthographicFar"].as<float>());
                }

                auto staticMeshComponent = entity["StaticMeshComponent"];
                if (staticMeshComponent)
                {
                    std::filesystem::path filename = staticMeshComponent["Filename"].as<std::string>();
                    
                    ref<Mesh> staticMesh = nullptr;
                    if (filename.has_extension())
                    {
                        staticMesh = AssetManagerOld::createMeshFromFile(filename.string());
                    }
                    else
                    {
                        staticMesh = AssetManagerOld::getMesh(filename.string());
                    }

                    auto& smc = deserializedEntity.addComponent<StaticMeshComponent>(staticMesh);

                    auto materials = staticMeshComponent["Materials"];
                    if (materials)
                    {
                        for (auto it = materials.begin(); it != materials.end(); ++it)
                        {
                            auto materialIndex = it->first.as<uint32_t>();
                            auto materialName  = it->second.as<std::string>();
                            smc.materials[materialIndex] = AssetManagerOld::getMaterial(materialName);
                        }
                    }
                }

                auto rigidbody3DComponent = entity["RigidBody3DComponent"];
                if (rigidbody3DComponent)
                {
                    auto stringToMotionType= [](const std::string& s) -> RigidBody3DComponent::MotionType
                    {
                        if (s == "Static")    return RigidBody3DComponent::MotionType::Static;
                        if (s == "Kinematic") return RigidBody3DComponent::MotionType::Kinematic;
                        if (s == "Dynamic")   return RigidBody3DComponent::MotionType::Dynamic;

                        MG_CORE_ASSERT_MSG(false, "Unknown motion type");
                        return RigidBody3DComponent::MotionType::Static;
                    };

                    auto& rb3d = deserializedEntity.addComponent<RigidBody3DComponent>();
                    rb3d.motionType           = stringToMotionType(rigidbody3DComponent["MotionType"].as<std::string>());
                    rb3d.friction             = rigidbody3DComponent["Friction"].as<float>();
                    rb3d.restitution          = rigidbody3DComponent["Restitution"].as<float>();
                    rb3d.linearDamping        = rigidbody3DComponent["LinearDamping"].as<float>();
                    rb3d.angularDamping       = rigidbody3DComponent["AngularDamping"].as<float>();
                    rb3d.isInitiallyActivated = rigidbody3DComponent["Activated"].as<bool>();
                }

                auto boxCollider3DComponent = entity["BoxCollider3DComponent"];
                if (boxCollider3DComponent)
                {
                    auto& bc3d      = deserializedEntity.addComponent<BoxCollider3DComponent>();
                    bc3d.offset     = boxCollider3DComponent["Offset"].as<glm::vec3>();
                    bc3d.halfExtent = boxCollider3DComponent["Extent"].as<glm::vec3>();
                }

                auto capsuleColliderComponent = entity["CapsuleColliderComponent"];
                if (capsuleColliderComponent)
                {
                    auto& cc      = deserializedEntity.addComponent<CapsuleColliderComponent>();
                    cc.offset     = capsuleColliderComponent["Offset"].as<glm::vec3>();
                    cc.halfHeight = capsuleColliderComponent["HalfHeight"].as<float>();
                    cc.radius     = capsuleColliderComponent["Radius"].as<float>();
                }

                auto sphereColliderComponent = entity["SphereColliderComponent"];
                if (sphereColliderComponent)
                {
                    auto& sc  = deserializedEntity.addComponent<SphereColliderComponent>();
                    sc.offset = sphereColliderComponent["Offset"].as<glm::vec3>();
                    sc.radius = sphereColliderComponent["Radius"].as<float>();
                }
            }

            // Loop again to resolve parent-child hierarchy
            MG_CORE_TRACE("Resolving scene graph...");
            for (auto entity : entities)
            {
                uint64_t uuid = entity["Entity"].as<uint64_t>();

                Entity parentEntity = scene->getEntity(uuid);

                auto transformComponent = entity["TransformComponent"];
                if (transformComponent)
                {
                    auto children = transformComponent["Children"];
                    if (children)
                    {
                        for (auto childUuid : children)
                        {
                            Entity childEntity = scene->getEntity(childUuid.as<uint64_t>());

                            // TODO(TG): figure out a better way to do this...
                            auto& childTc = childEntity.getTransform();
                            auto position = childTc.getLocalPosition();
                            auto rotation = childTc.getLocalRotation();
                            auto scale    = childTc.getLocalScale();

                            childTc.setLocalPosition(0, 0, 0);
                            childTc.setLocalRotation(0, 0, 0);
                            childTc.setLocalScale   (1, 1, 1);

                            parentEntity.addChild(childEntity);
                            childTc.setLocalPosition(position);
                            childTc.setLocalRotation(rotation);
                            childTc.setLocalScale(scale);
                        }
                    }
                }                
            }
        }

        MG_CORE_TRACE("Successfully loaded scene '{}'", sceneName);
        return scene;
    }
}