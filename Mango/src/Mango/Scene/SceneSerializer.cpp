#include "mgpch.h"

#include "Entity.h"
#include "SceneSerializer.h"
#include "Mango/Core/AssetManager.h"

#include <yaml-cpp/yaml.h>
#include <strutil.h>
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
                out << YAML::Key << "Position"    << YAML::Value << tc.getPosition();
                out << YAML::Key << "Rotation"    << YAML::Value << glm::degrees(tc.getRotation());
                out << YAML::Key << "Orientation" << YAML::Value << tc.getOrientation();
                out << YAML::Key << "Scale"       << YAML::Value << tc.getScale();
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

        if (entity.hasComponent<ModelRendererComponent>())
        {
            auto modelTypeToString = [](Model::ModelType type) -> std::string
            {
                switch (type)
                {
                    case Model::ModelType::Model3D:  return "Model3D";
                    case Model::ModelType::Cone:     return "Cone";
                    case Model::ModelType::Cube:     return "Cube";
                    case Model::ModelType::Cylinder: return "Cylinder";
                    case Model::ModelType::Plane:    return "Plane";
                    case Model::ModelType::Sphere:   return "Sphere";
                    case Model::ModelType::Torus:    return "Torus";
                    case Model::ModelType::Quad:     return "Quad";
                }
                MG_CORE_ASSERT_MSG(false, "Unknown model type");
                return {};
            };

            auto renderQueueToString = [](ModelRendererComponent::RenderQueue queue) -> std::string
            {
                switch (queue)
                {
                    case ModelRendererComponent::RenderQueue::RQ_OPAQUE:                 return "Opaque";
                    case ModelRendererComponent::RenderQueue::RQ_ALPHA:                  return "Alpha";
                    case ModelRendererComponent::RenderQueue::RQ_ENVIRO_MAPPING_STATIC:  return "StaticEnvMapping";
                    case ModelRendererComponent::RenderQueue::RQ_ENVIRO_MAPPING_DYNAMIC: return "DynamicEnvMapping";
                }
                MG_CORE_ASSERT_MSG(false, "Unknown render queue type");
                return {};
            };

            auto materialTextureTypeToString = [](Material::TextureType type) -> std::string
            {
                switch (type)
                {
                    case Material::TextureType::DIFFUSE:  return "Diffuse";
                    case Material::TextureType::SPECULAR: return "Specular";
                    case Material::TextureType::NORMAL:   return "Normal";
                    case Material::TextureType::EMISSION: return "Emission";
                    case Material::TextureType::DEPTH:    return "Depth";
                }
                MG_CORE_ASSERT_MSG(false, "Unknown texture type");
                return {};
            };

            out << YAML::Key << "ModelRendererComponent";
            out << YAML::BeginMap;
            {
                auto& mrc = entity.getComponent<ModelRendererComponent>();
                out << YAML::Key << "ModelType"   << YAML::Value << modelTypeToString(mrc.model.getModelType());
                out << YAML::Key << "RenderQueue" << YAML::Value << renderQueueToString(mrc.getRenderQueue());
                
                if (mrc.model.getModelType() == Model::ModelType::Model3D)
                {
                    out << YAML::Key << "Filename"  << YAML::Value << mrc.model.m_filename;
                }
                else
                {
                    out << YAML::Key << "PrimitiveProperties" << YAML::Value;
                    out << YAML::BeginMap;
                    {
                        auto pp = mrc.model.getPrimitiveProperties();
                        out << YAML::Key << "Width"       << YAML::Value << pp.width;
                        out << YAML::Key << "Height"      << YAML::Value << pp.height;
                        out << YAML::Key << "Radius"      << YAML::Value << pp.radius;
                        out << YAML::Key << "Size"        << YAML::Value << pp.size;
                        out << YAML::Key << "InnerRadius" << YAML::Value << pp.innerRadius;
                        out << YAML::Key << "OuterRadius" << YAML::Value << pp.outerRadius;
                        out << YAML::Key << "Slices"      << YAML::Value << pp.slices;
                        out << YAML::Key << "Stacks"      << YAML::Value << pp.stacks;
                    }
                    out << YAML::EndMap;

                    out << YAML::Key << "Material" << YAML::Value;
                    out << YAML::BeginMap;
                    {
                        auto material = mrc.model.getMesh().material;

                        out << YAML::Key << "Vec3Map" << YAML::Value;
                        out << YAML::BeginMap;
                        {
                            for (auto& [propertyName, vec3Value] : material.getVec3Map())
                            {
                                out << YAML::Key << propertyName << YAML::Value << vec3Value;
                            }
                        }
                        out << YAML::EndMap;

                        out << YAML::Key << "FloatMap" << YAML::Value;
                        out << YAML::BeginMap;
                        {
                            for (auto& [propertyName, floatValue] : material.getFloatMap())
                            {
                                out << YAML::Key << propertyName << YAML::Value << floatValue;
                            }
                        }
                        out << YAML::EndMap;

                        out << YAML::Key << "Textures" << YAML::Value;
                        out << YAML::BeginMap;
                        {
                            for (auto& [textureType, texture] : material.getTexturesMap())
                            {
                                out << YAML::Key << materialTextureTypeToString(textureType) << YAML::Value << texture->getFilename();
                            }
                        }
                        out << YAML::EndMap;
                    }
                    out << YAML::EndMap;
                }
                out << YAML::EndMap;
            }
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

        auto pathString = outFilepath.string();
        strutil::replace_all(pathString, "\\", "/");

        std::ofstream fout(pathString);
        if (fout.is_open())
        {
            fout << out.c_str();
            return true;
        }
        else
        {
            MG_CORE_ERROR("Can't save file {}", pathString);
        }

        return false;
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
            MG_CORE_ERROR("Failed to load scene file '{}'\n    {}", inFilepath.string(), e.what());
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
                    tc.setPosition(transformComponent["Position"].as<glm::vec3>());
                    tc.setRotation(glm::radians(transformComponent["Rotation"].as<glm::vec3>()));
                    tc.setScale   (transformComponent["Scale"].as<glm::vec3>());
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

                auto modelRendererComponent = entity["ModelRendererComponent"];
                if (modelRendererComponent)
                {
                    auto stringToModelType = [](const std::string& s) -> Model::ModelType
                    {
                        if (s == "Model3D")  return Model::ModelType::Model3D;
                        if (s == "Cone")     return Model::ModelType::Cone;
                        if (s == "Cube")     return Model::ModelType::Cube;
                        if (s == "Cylinder") return Model::ModelType::Cylinder;
                        if (s == "Plane")    return Model::ModelType::Plane;
                        if (s == "Sphere")   return Model::ModelType::Sphere;
                        if (s == "Torus")    return Model::ModelType::Torus;
                        if (s == "Quad")     return Model::ModelType::Quad;

                        MG_CORE_ASSERT_MSG(false, "Unknown model type");
                        return Model::ModelType::Cube;
                    };

                    auto stringToRenderQueue = [](const std::string& s) -> ModelRendererComponent::RenderQueue
                    {
                        if (s == "Opaque")            return ModelRendererComponent::RenderQueue::RQ_OPAQUE;
                        if (s == "Alpha")             return ModelRendererComponent::RenderQueue::RQ_ALPHA;
                        if (s == "StaticEnvMapping")  return ModelRendererComponent::RenderQueue::RQ_ENVIRO_MAPPING_STATIC;
                        if (s == "DynamicEnvMapping") return ModelRendererComponent::RenderQueue::RQ_ENVIRO_MAPPING_DYNAMIC;

                        MG_CORE_ASSERT_MSG(false, "Unknown render queue");
                        return ModelRendererComponent::RenderQueue::RQ_OPAQUE;
                    };

                    auto stringToMaterialTextureType = [](const std::string& s) -> Material::TextureType
                    {
                        if (s == "Diffuse")  return Material::TextureType::DIFFUSE;
                        if (s == "Specular") return Material::TextureType::SPECULAR;
                        if (s == "Normal")   return Material::TextureType::NORMAL;
                        if (s == "Emission") return Material::TextureType::EMISSION;
                        if (s == "Depth")    return Material::TextureType::DEPTH;

                        MG_CORE_ASSERT_MSG(false, "Unknown texture type");
                        return Material::TextureType::DIFFUSE;
                    };

                    auto modelType = stringToModelType(modelRendererComponent["ModelType"].as<std::string>());
                    auto renderQueue = stringToRenderQueue(modelRendererComponent["RenderQueue"].as<std::string>());

                    if (modelType == Model::ModelType::Model3D)
                    {
                        auto filename = modelRendererComponent["Filename"].as<std::string>();
                        Model model(filename);

                        deserializedEntity.addComponent<ModelRendererComponent>(model, renderQueue);
                    }
                    else
                    {
                        Model model("");
                        auto  primitiveProperties = modelRendererComponent["PrimitiveProperties"];

                        PrimitiveProperties pp;
                        pp.width       = primitiveProperties["Width"].as<float>();
                        pp.height      = primitiveProperties["Height"].as<float>();
                        pp.radius      = primitiveProperties["Radius"].as<float>();
                        pp.size        = primitiveProperties["Size"].as<float>();
                        pp.innerRadius = primitiveProperties["InnerRadius"].as<float>();
                        pp.outerRadius = primitiveProperties["OuterRadius"].as<float>();
                        pp.slices      = primitiveProperties["Slices"].as<int32_t>();
                        pp.stacks      = primitiveProperties["Stacks"].as<int32_t>();
                        model.setPrimitiveProperties(modelType, pp);

                        deserializedEntity.addComponent<ModelRendererComponent>(model, renderQueue);

                        auto material = modelRendererComponent["Material"];
                        if (material)
                        {
                            auto& mrc          = deserializedEntity.getComponent<ModelRendererComponent>();
                            auto& meshMaterial = mrc.model.getMesh().material;

                            auto vec3Map = material["Vec3Map"];
                            if (vec3Map)
                            {   
                                for (auto it = vec3Map.begin(); it != vec3Map.end(); ++it)
                                {
                                    meshMaterial.addVector3(it->first.as<std::string>(), it->second.as<glm::vec3>());
                                }
                            }

                            auto floatMap = material["FloatMap"];
                            if (floatMap)
                            {
                                for (auto it = floatMap.begin(); it != floatMap.end(); ++it)
                                {
                                    meshMaterial.addFloat(it->first.as<std::string>(), it->second.as<float>());
                                }
                            }

                            auto texturesMap = material["Textures"];
                            if (texturesMap)
                            {
                                for (auto it = texturesMap.begin(); it != texturesMap.end(); ++it)
                                {
                                    auto textureFilename = it->second.as<std::string>();

                                    if (!textureFilename.empty())
                                    {
                                        auto textureType = stringToMaterialTextureType(it->first.as<std::string>());
                                        auto texture = AssetManager::createTexture2D(textureFilename, (textureType == Material::TextureType::DIFFUSE) ? true : false);

                                        meshMaterial.addTexture(textureType, texture);
                                    }
                                }
                            }
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
                    auto& bc3d = deserializedEntity.addComponent<BoxCollider3DComponent>();
                    bc3d.offset     = boxCollider3DComponent["Offset"].as<glm::vec3>();
                    bc3d.halfExtent = boxCollider3DComponent["Extent"].as<glm::vec3>();
                }

                auto sphereColliderComponent = entity["SphereColliderComponent"];
                if (sphereColliderComponent)
                {
                    auto& sc = deserializedEntity.addComponent<SphereColliderComponent>();
                    sc.offset = sphereColliderComponent["Offset"].as<glm::vec3>();
                    sc.radius = sphereColliderComponent["Radius"].as<float>();
                }
            }
        }

        return scene;
    }
}