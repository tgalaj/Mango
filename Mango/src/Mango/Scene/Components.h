#pragma once

#include "Entity.h"

#include "Mango/Core/AssetManager.h"
#include "Mango/Core/Services.h"
#include "Mango/Core/UUID.h"

#include "Mango/Math/Math.h"

#include "Mango/Rendering/Attenuation.h"
#include "Mango/Rendering/Mesh.h"
#include "Mango/Rendering/Camera/Camera.h"

#ifndef GLM_ENABLE_EXPERIMENTAL
    #define GLM_ENABLE_EXPERIMENTAL
#endif

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/component_wise.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

#include <memory>
#include <typeinfo>
#include <vector>

namespace mango
{
    struct IDComponent
    {
        IDComponent()                   = default;
        IDComponent(const IDComponent&) = default;
        IDComponent(UUID uuid) : id(uuid) {}

        UUID id;
    };

    struct TagComponent
    {
        TagComponent() = default;
        TagComponent(const std::string& name)
            : name(name) {}

        std::string name;
    };

    struct ShadowInfo
    {
    public:
        explicit ShadowInfo(const glm::mat4& projection = glm::mat4(1.0f), bool castsShadows = true)
            : m_projection  (projection),
              m_castsShadows(castsShadows) {}

        ~ShadowInfo() {}

        glm::mat4 getProjection()   const { return m_projection; }
        bool      getCastsShadows() const { return m_castsShadows; }

        void setCastsShadows(bool castsShadows) { m_castsShadows = castsShadows; }

    private:
        glm::mat4 m_projection{};
        bool      m_castsShadows;
    };

    struct BaseLight
    {
    public:
        BaseLight(const glm::vec3& color, float intensity, bool castsShadows = false)
            : color         (color),
              intensity     (intensity),
              m_shadowInfo  (ShadowInfo(glm::mat4(1.0f), castsShadows)) {}

        virtual ~BaseLight() {}

        ShadowInfo getShadowInfo() const { return m_shadowInfo; }

        void setCastsShadows(bool castsShadows)       { m_shadowInfo.setCastsShadows(castsShadows); }
        bool getCastsShadows()                  const { return m_shadowInfo.getCastsShadows(); }

        glm::vec3 color{};
        float     intensity = 1.0f;

    protected:
        void setShadowInfo(const ShadowInfo& shadowInfo) { m_shadowInfo = shadowInfo; }
    
    protected:
        ShadowInfo m_shadowInfo;
    };

    struct DirectionalLightComponent : public BaseLight
    {
    public:
        DirectionalLightComponent(const glm::vec3 & color, float intensity, float size = 20.0f, bool castsShadows = false)
            : BaseLight(color, intensity),
              m_size            (size)
        {
            m_shadowInfo = ShadowInfo(glm::ortho(-m_size, m_size, -m_size, m_size, -m_size, m_size), castsShadows);
        }

        explicit DirectionalLightComponent()
            : BaseLight(glm::vec3(1.0f), 1.0f)
        {}

        void setSize(float size)
        { 
            m_size = size; 
            setShadowInfo(ShadowInfo(glm::ortho(-m_size, m_size, -m_size, m_size, -m_size, m_size), m_shadowInfo.getCastsShadows()));
        }

        float getSize() const { return m_size; }

    private:
        float m_size = 20.0f;
    };

    struct PointLightComponent : public BaseLight
    {
    public:
        PointLightComponent(bool castsShadows = false)
            : BaseLight(glm::vec3(1.0f), 1.0f),
              m_attenuation     (0.0, 0.0f, 1.0f)
        {
            calculateRange();
            recalculateShadowInfo(castsShadows);
        }

        PointLightComponent(const glm::vec3 &   color, 
                                  float         intensity, 
                            const Attenuation & attenuation,
                                  bool          castsShadows = false)
            : BaseLight(color, intensity, castsShadows),
              m_attenuation     (attenuation)
        {
            calculateRange();
            recalculateShadowInfo(castsShadows);
        }

        void setAttenuation(float constant, float linear, float quadratic)
        {
            m_attenuation = Attenuation(constant, linear, quadratic);
            calculateRange();
        }

        Attenuation getAttenuation() const { return m_attenuation; }

        void  setShadowNearPlane(float shadowNearPlane)       { m_shadowNearPlane = shadowNearPlane; recalculateShadowInfo(m_shadowInfo.getCastsShadows()); }
        float getShadowNearPlane()                      const { return m_shadowNearPlane; }

        void  setShadowFarPlane(float shadowFarPlane)        { m_shadowFarPlane = shadowFarPlane; recalculateShadowInfo(m_shadowInfo.getCastsShadows()); }
        float getShadowFarPlane()                      const { return m_shadowFarPlane; }

        float getRange() const { return m_range; }

    protected:
        static const int COLOR_DEPTH = 256;

        void calculateRange()
        {
            // This is wrong, temporary solution...
            float a = m_attenuation.quadratic;
            float b = m_attenuation.linear;
            float c = m_attenuation.constant - (float)COLOR_DEPTH * glm::compMax(color);

            m_range = (-b + glm::sqrt(b * b - 4 * a * c)) / (2 * a);
        }

        void recalculateShadowInfo(bool castsShadows)
        {
            m_shadowInfo = ShadowInfo(glm::perspective(m_cutoff, 1.0f, m_shadowNearPlane, m_shadowFarPlane), castsShadows);
        }

    protected:
        Attenuation m_attenuation;
        float       m_range{};
        float       m_shadowNearPlane = 0.001f;
        float       m_shadowFarPlane  = 100.0f;
        float       m_cutoff          = glm::radians(90.0f);
    };

    struct SpotLightComponent : public PointLightComponent
    {
    public:
        SpotLightComponent()
        {   
            setCutOffAngle(30.0f);
        }

        SpotLightComponent(const glm::vec3   & color, 
                                 float         intensity, 
                           const Attenuation & attenuation, 
                                 float         cutoff,
                                 bool          castsShadows = false)
            : PointLightComponent(color, intensity, attenuation, castsShadows)
        {
            m_shadowInfo.setCastsShadows(castsShadows);
            setCutOffAngle(cutoff);
        }

        /** Sets the cutoff angle in degrees. */
        void setCutOffAngle(float angle)
        {
            m_cutoff = glm::radians(angle);
            recalculateShadowInfo(m_shadowInfo.getCastsShadows());
        }

        /** Returns the cutoff angle in radians. */
        float getCutOffAngle() const { return m_cutoff; }
    };

    struct CameraComponent
    {
        Camera camera;
        bool isPrimary = false;
    };

    struct StaticMeshComponent
    {
    public:
        StaticMeshComponent()
        {
            if (materials.empty())
            {
                materials.emplace_back(AssetManagerOld::getMaterial("DefaultMaterial"));
            }
        }

        explicit StaticMeshComponent(const ref<Mesh>& m)
            : mesh(m) 
        {
            if (mesh) materials = mesh->getMaterials();
        }

    public:
        ref<Mesh>     mesh      = nullptr;
        MaterialTable materials;// = {};
    };

    #if 0
    struct AnimatedMeshComponent
    {
    public:
        AnimatedMeshComponent()
        {}

        explicit AnimatedMeshComponent(const ref<AnimatedMesh>& animatedMesh)
            : animatedMesh(animatedMesh)
        {}

    public:
        ref<AnimatedMesh> animatedMesh;
    };
    #endif

    struct TransformComponent
    {
    public:
        explicit TransformComponent(const glm::vec3 & position = glm::vec3(0.0f),
                                    const glm::vec3 & rotation = glm::vec3(0.0f, 0.0f, 0.0f),
                                    const glm::vec3 & scale    = glm::vec3(1.0f))
            : m_localOrientation(rotation),
              m_localPosition   (position),
              m_localRotation   (rotation),
              m_localScale      (scale),
              m_dirty           (true),
              m_worldMatrix     (glm::mat4(1.0f)),
              m_normalMatrix    (glm::mat3(1.0f))
        {}

        //// Local Space Setters

        void setLocalPosition(const glm::vec3 & position)
        {
            m_localPosition = position;
            m_dirty         = true;
        }

        void setLocalPosition(float x, float y, float z)
        {
            setLocalPosition(glm::vec3(x, y, z));
        }

        /*
         * Set rotation using Euler Angles in radians
         */
        void setLocalRotation(const glm::vec3& euler)
        {
            m_localRotation    = euler;
            m_localOrientation = glm::quat(m_localRotation);
            m_localOrientation = glm::normalize(m_localOrientation);
            m_dirty            = true;
        }

        /*
         * Set rotation using Euler Angles in degrees
         */
        void setLocalRotation(float x, float y, float z)
        {
            setLocalRotation(glm::radians(glm::vec3(x, y, z)));
        }

        /*
        * Set orientation using axis and angle in radians
        */
        void setLocalRotation(const glm::vec3 & axis, float angle)
        {
            m_localOrientation = glm::normalize(glm::angleAxis(angle, glm::normalize(axis)));
            m_localRotation    = glm::eulerAngles(m_localOrientation);
            m_dirty            = true;
        }

        void setLocalRotation(const glm::quat & quat)
        {
            m_localOrientation = glm::normalize(quat);
            m_localRotation    = glm::eulerAngles(m_localOrientation);
            m_dirty            = true;
        }

        void setLocalScale(float x, float y, float z)
        {
            m_localScale = glm::vec3(x, y, z);
            m_dirty      = true;
        }

        void setLocalScale(float uniformScale)
        {
            m_localScale = glm::vec3(uniformScale);
            m_dirty      = true;
        }

        void setLocalScale(const glm::vec3 & scale)
        {
            m_localScale = scale;
            m_dirty      = true;
        }

        //// World Space Setters

        void setPosition(const glm::vec3 & position)
        {
            m_worldMatrix[3].x = position.x;
            m_worldMatrix[3].y = position.y;
            m_worldMatrix[3].z = position.z;

            setLocalPosition(glm::inverse(m_parentWorldMatrix) * glm::vec4(position, 1.0));
        }

        void setPosition(float x, float y, float z)
        {
            setPosition(glm::vec3(x, y, z));
        }

        /*
         * Set rotation using Euler Angles in radians
         */
        void setRotation(const glm::vec3& euler)
        {
            m_worldMatrix = TRS(getPosition(), euler, getScale());

            // TODO: it's ugly, maybe there's another, simpler way?
            auto m   = glm::inverse(m_parentWorldMatrix) * m_worldMatrix;
            auto rot = math::decomposeRotation(m);
            setLocalRotation(rot);
        }

        /*
         * Set rotation using Euler Angles in degrees
         */
        void setRotation(float x, float y, float z)
        {
            setRotation(glm::radians(glm::vec3(x, y, z)));
        }

        /*
        * Set orientation using axis and angle in radians
        */
        void setRotation(const glm::vec3 & axis, float angle)
        {
            glm::quat rotation      = glm::angleAxis(angle, glm::normalize(axis));
                      m_worldMatrix = TRS(getPosition(), rotation, getScale());

            // TODO: it's ugly, maybe there's another, simpler way?
            auto m   = glm::inverse(m_parentWorldMatrix) * m_worldMatrix;
            auto rot = math::decomposeRotation(m);
            setLocalRotation(rot);
        }

        void setRotation(const glm::quat & quat)
        {
            m_worldMatrix = TRS(getPosition(), quat, getScale());
            
            // TODO: it's ugly, maybe there's another, simpler way?
            auto m   = glm::inverse(m_parentWorldMatrix) * m_worldMatrix;
            auto rot = math::decomposeRotation(m);
            setLocalRotation(rot);
        }

        //// Hierarchy

        auto&  getChildren()       { return m_children;                     }
        Entity getParent  () const { return m_parent;                       }
        bool   hasParent  () const { return m_parent != Entity::nullEntity; }
        
        void addChild   (Entity childEntity, Entity parentEntity);
        void removeChild(Entity childEntity);
        void resetParent();
        void update     (const glm::mat4 & parentTransform, bool dirty);
        
        //// Matrices

        glm::mat4 getWorldMatrix      () const { return m_worldMatrix;       }
        glm::mat4 getLocalWorldMatrix () const { return m_localWorldMatrix;  }
        glm::mat4 getParentWorldMatrix() const { return m_parentWorldMatrix; }
        glm::mat3 getNormalMatrix     () const { return m_normalMatrix;      }

        //// Local Space Getters

        glm::vec3 getLocalPosition   () const { return m_localPosition;     }
        glm::quat getLocalOrientation() const { return m_localOrientation;  }
        /** Returns rotation in radians. */
        glm::vec3 getLocalRotation   () const { return m_localRotation;     }
        glm::vec3 getLocalScale      () const { return m_localScale;        }

        //// World Space Getters

        glm::vec3 getPosition   () const { return m_worldMatrix[3]; }
        glm::quat getOrientation() const { return glm::normalize(glm::quat(getRotation())); }
        /** Returns rotation in radians. */
        glm::vec3 getRotation   () const { return  math::decomposeRotation(m_worldMatrix);  }
        glm::vec3 getScale      () const 
        { 
            return  { glm::length(m_worldMatrix[0]), glm::length(m_worldMatrix[1]), glm::length(m_worldMatrix[2]) };
        }
        
        //// Directions

        glm::vec3 getForward               () const { return glm::normalize( m_worldMatrix[2]); }
        glm::vec3 getBackward              () const { return glm::normalize(-m_worldMatrix[2]); }
        glm::vec3 getRight                 () const { return glm::normalize( m_worldMatrix[0]); }
        glm::vec3 getUp                    () const { return glm::normalize( m_worldMatrix[1]); }
        glm::vec3 getLocalOrientationVector() const { return glm::normalize(glm::conjugate(m_localOrientation) * glm::vec3(0.0f, 0.0f, 1.0f)); }
        glm::vec3 getOrientationVector     () const { return glm::normalize(glm::conjugate(getOrientation())   * glm::vec3(0.0f, 0.0f, 1.0f)); }

        //// Helpers

        glm::mat4 TRS(const glm::vec3& translation, const glm::vec3& euler,    const glm::vec3& scale);
        glm::mat4 TRS(const glm::vec3& translation, const glm::quat& rotation, const glm::vec3& scale);

    private:
        std::vector<Entity> m_children = {};
        Entity              m_parent   = Entity::nullEntity;

        glm::mat4 m_worldMatrix      { 1.0f };
        glm::mat4 m_localWorldMatrix { 1.0f };
        glm::mat4 m_parentWorldMatrix{ 1.0f };
        glm::mat3 m_normalMatrix     { 1.0f };

        // Local Space - relative to the parent's transform
        glm::quat m_localOrientation{};
        glm::vec3 m_localPosition   {};
        glm::vec3 m_localRotation   {};
        glm::vec3 m_localScale      { 1.0f };

        bool m_dirty = true;
    
    private:
        friend class SceneSerializer;
    };

    // Physics

    struct RigidBody3DComponent
    {
        enum class MotionType : uint8_t { Static, Kinematic, Dynamic };
        MotionType motionType = MotionType::Static;

        float friction       = 0.2f;
        float restitution    = 0.0f;
        float linearDamping  = 0.05f;
        float angularDamping = 0.05f;

        bool isInitiallyActivated = true;

    protected:
        void* runtimeBody = nullptr;

    private:
        friend class PhysicsSystem;
    };

    struct BoxCollider3DComponent
    {
        BoxCollider3DComponent() = default;
        BoxCollider3DComponent(const glm::vec3& halfExtent, 
                               const glm::vec3& offset = { 0.0f, 0.0f, 0.0f }) 
            : halfExtent(halfExtent),
              offset    (offset){}

        glm::vec3 offset     = { 0.0f, 0.0f, 0.0f };
        glm::vec3 halfExtent = { 0.5f, 0.5f, 0.5f };
    };

    struct CapsuleColliderComponent
    {
        CapsuleColliderComponent() = default;
        CapsuleColliderComponent(      float      halfHeight,
                                       float      radius,
                                 const glm::vec3& offset = { 0.0f, 0.0f, 0.0f })
            : offset    (offset),
              halfHeight(halfHeight),
              radius    (radius)
        {}

        glm::vec3 offset     = { 0.0f, 0.0f, 0.0f };
        float     halfHeight = 0.5f;
        float     radius     = 0.5f;
    };

    struct SphereColliderComponent
    {
        SphereColliderComponent() = default;
        SphereColliderComponent(      float      radius,
                                const glm::vec3& offset = { 0.0f, 0.0f, 0.0f })
            : radius(radius),
              offset(offset)
        {}

        glm::vec3 offset = { 0.0f, 0.0f, 0.0f };
        float     radius = 0.5f;
    };

    // Components Registry
    template<typename... Component>
    struct ComponentsGroup
    {
    };

    // Except: IDComponent, TagComponent which are special case components
    using ComponentsRegistry = ComponentsGroup<DirectionalLightComponent, PointLightComponent, SpotLightComponent, 
                                               CameraComponent, StaticMeshComponent, /*AnimatedMeshComponent,*/ TransformComponent, 
                                               RigidBody3DComponent, BoxCollider3DComponent, CapsuleColliderComponent, SphereColliderComponent>;
}