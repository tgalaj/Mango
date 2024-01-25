#pragma once

#include "Mango/Core/UUID.h"
#include "Mango/Rendering/Attenuation.h"
#include "Mango/Rendering/Model.h"

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
        TagComponent(const std::string& tag)
            : tag(tag) {}

        std::string tag;
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

    struct BaseLightComponent
    {
    public:
        BaseLightComponent(const glm::vec3& color, float intensity, bool castsShadows = false)
            : color         (color),
              intensity     (intensity),
              m_shadowInfo  (ShadowInfo(glm::mat4(1.0f), castsShadows)) {}

        virtual ~BaseLightComponent() {}

        ShadowInfo getShadowInfo() const { return m_shadowInfo; }

        void setCastsShadows(bool castsShadows)       { m_shadowInfo.setCastsShadows(castsShadows); }
        bool getCastsShadows()                  const { return m_shadowInfo.getCastsShadows(); }

        glm::vec3 color{};
        float     intensity;

    protected:
        void setShadowInfo(const ShadowInfo& shadowInfo) { m_shadowInfo = shadowInfo; }
    
    protected:
        ShadowInfo m_shadowInfo;
    };

    struct DirectionalLightComponent : public BaseLightComponent
    {
    public:
        DirectionalLightComponent(const glm::vec3 & color, float intensity, float size = 20.0f, bool castsShadows = false)
            : BaseLightComponent(color, intensity)
        {
            m_shadowInfo = ShadowInfo(glm::ortho(-size, size, -size, size, -size, size), castsShadows);
        }

        explicit DirectionalLightComponent()
            : BaseLightComponent(glm::vec3(1.0f), 1.0f)
        {}

        void setSize(float size)
        { 
            m_size = size; 
            setShadowInfo(ShadowInfo(glm::ortho(-size, size, -size, size, -size, size), m_shadowInfo.getCastsShadows()));
        }

        float getSize() const { return m_size; }

    private:
        float m_size = 20.0f;
    };

    struct PointLightComponent : public BaseLightComponent
    {
    public:
        PointLightComponent(bool castsShadows = false)
            : BaseLightComponent(glm::vec3(1.0f), 1.0f),
              m_attenuation     (0.0, 0.0f, 1.0f)
        {
            calculateRange();
            recalculateShadowInfo(castsShadows);
        }

        PointLightComponent(const glm::vec3 &   color, 
                                  float         intensity, 
                            const Attenuation & attenuation,
                                  bool          castsShadows = false)
            : BaseLightComponent(color, intensity, castsShadows),
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
        static const int COLOR_DEPTH = 255;

        void calculateRange()
        {
            float a = m_attenuation.quadratic;
            float b = m_attenuation.linear;
            float c = m_attenuation.constant - COLOR_DEPTH * intensity * glm::compMax(color);

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
    public:
        enum class ProjectionType { Perspective = 0, Orthographic = 1 };
    public:
        CameraComponent();
        
        /** verticalFov in degrees. */
        void setPerspective(float verticalFov, float aspectRatio, float nearClip, float farClip);
        void setOrtographic(float size,        float aspectRatio, float nearClip, float farClip);

        /** Returns vertical field of view angle in radians. */
        float getPerspectiveVerticalFieldOfView() const { return m_perspectiveFov; }
        
        /** verticalFov in degrees. */
        void  setPerspectiveVerticalFieldOfView(float verticalFov) { m_perspectiveFov = glm::radians(verticalFov); recalculateProjection(); }
        
        float getPerspectiveNearClip() const         { return m_perspectiveNear; }
        void  setPerspectiveNearClip(float nearClip) { m_perspectiveNear = nearClip; recalculateProjection(); }
        
        float getPerspectiveFarClip() const        { return m_perspectiveFar; }
        void  setPerspectiveFarClip(float farClip) { m_perspectiveFar = farClip; recalculateProjection(); }

        float getOrthographicSize() const     { return m_orthographicSize; }
        void  setOrthographicSize(float size) { m_orthographicSize = size; recalculateProjection(); }
        
        float getOrthographicNearClip() const         { return m_orthographicNear; }
        void  setOrthographicNearClip(float nearClip) { m_orthographicNear = nearClip; recalculateProjection(); }
        
        float getOrthographicFarClip() const        { return m_orthographicFar; }
        void  setOrthographicFarClip(float farClip) { m_orthographicFar = farClip; recalculateProjection(); }

        ProjectionType getProjectionType() const    { return m_projectionType; }
        void setProjectionType(ProjectionType type) { m_projectionType = type; recalculateProjection(); }

        bool isPrimary() const { return m_isPrimary; }
        void setPrimary();

        const glm::mat4& view()       const { return m_view; }
        const glm::mat4& projection() const { return m_projection; }

        void setView(const glm::mat4& view) { m_view = view; }

    private:
        void recalculateProjection();

    private:
        glm::mat4 m_view{};
        glm::mat4 m_projection{};

        float m_perspectiveFov  = glm::radians(60.0f);
        float m_perspectiveNear = 0.01f, m_perspectiveFar = 1000.0f;

        float m_orthographicSize = 10.0f;
        float m_orthographicNear = 0.01f, m_orthographicFar = 1000.0f;

        float m_aspectRatio = 1.0f;

        ProjectionType m_projectionType = ProjectionType::Perspective;
        bool           m_isPrimary      = false;
    
    private:
        friend class Scene;
    };

    struct ModelRendererComponent
    {
    public:
        enum class RenderQueue { RQ_OPAQUE, RQ_ALPHA, RQ_ENVIRO_MAPPING_STATIC, RQ_ENVIRO_MAPPING_DYNAMIC };

        ModelRendererComponent()
            : m_renderQueue(RenderQueue::RQ_OPAQUE)
        {}

        explicit ModelRendererComponent(const Model& model, RenderQueue renderQueue = RenderQueue::RQ_OPAQUE)
            : model(model),
              m_renderQueue(renderQueue)
        {}

        RenderQueue getRenderQueue() const { return m_renderQueue; }

    public:
        Model model;

    private:
        RenderQueue m_renderQueue;
    };

    struct TransformComponent
    {
    public:
        explicit TransformComponent(const glm::vec3 & position = glm::vec3(0.0f),
                                    const glm::vec3 & rotation = glm::vec3(0.0f, 0.0f, 0.0f),
                                    const glm::vec3 & scale    = glm::vec3(1.0f))
            : m_orientation (rotation),
              m_position    (position),
              m_rotation    (rotation),
              m_scale       (scale),
              m_isDirty     (true),
              m_worldMatrix (glm::mat4(1.0f)),
              m_normalMatrix(glm::mat3(1.0f)),
              m_direction   (glm::vec3(0.0f, 0.0f, -1.0f))
        {}

        void setPosition(float x, float y, float z)
        {
            m_position = glm::vec3(x, y, z);
            m_isDirty  = true;
        }

        void setPosition(const glm::vec3 & position)
        {
            m_position = position;
            m_isDirty  = true;
        }

        /*
         * Set rotation using Euler Angles in degrees
         */
        void setRotation(const glm::vec3& euler)
        {
            m_rotation    = glm::radians(euler);
            m_orientation = glm::quat(m_rotation);
            m_direction   = glm::normalize(glm::conjugate(m_orientation) * glm::vec3(0.0f, 0.0f, 1.0f));
            m_isDirty     = true;
        }

        /*
         * Set rotation using Euler Angles in degrees
         */
        void setRotation(float x, float y, float z)
        {
            setRotation(glm::vec3(x, y, z));
        }

        /*
        * Set orientation using axis and angle in degrees
        */
        void setOrientation(const glm::vec3 & axis, float angle)
        {
            m_orientation = glm::normalize(glm::angleAxis(glm::radians(angle), glm::normalize(axis)));
            m_rotation    = glm::eulerAngles(m_orientation);
            m_direction   = glm::normalize(glm::conjugate(m_orientation) * glm::vec3(0.0f, 0.0f, 1.0f));
            m_isDirty     = true;
        }

        void setOrientation(const glm::quat & quat)
        {
            m_orientation = glm::normalize(quat);
            m_direction   = glm::normalize(glm::conjugate(m_orientation) * glm::vec3(0.0f, 0.0f, 1.0f));
            m_rotation    = glm::eulerAngles(m_orientation);
            m_isDirty     = true;
        }

        void setScale(float x, float y, float z)
        {
            m_scale   = glm::vec3(x, y, z);
            m_isDirty = true;
        }

        void setScale(float uniformScale)
        {
            m_scale   = glm::vec3(uniformScale);
            m_isDirty = true;
        }

        void setScale(const glm::vec3 & scale)
        {
            m_scale   = scale;
            m_isDirty = true;
        }

        void addChild(TransformComponent& child)
        {
            m_children.push_back(&child);
        }

        void update(const glm::mat4 & parentTransform, bool dirty)
        {
            dirty |= m_isDirty;

            if(dirty)
            {
                m_worldMatrix  = getUpdatedWorldMatrix();
                m_worldMatrix  = parentTransform * m_worldMatrix;
                m_normalMatrix = glm::mat3(glm::transpose(glm::inverse(m_worldMatrix)));

                m_isDirty = false;
            }

            for(unsigned i = 0; i < m_children.size(); ++i)
            {
                m_children[i]->update(m_worldMatrix, dirty);
            }
        }

        glm::mat4 getWorldMatrix () const { return m_worldMatrix;  }
        glm::mat3 getNormalMatrix() const { return m_normalMatrix; }
        glm::quat getOrientation () const { return m_orientation;  }
        glm::vec3 getPosition    () const { return m_position;     }
        
        /** Returns rotation in radians. */
        glm::vec3 getRotation    () const { return m_rotation;     }
        glm::vec3 getScale       () const { return m_scale;        }
        glm::vec3 getDirection   () const { return m_direction;    }

    private:
        glm::mat4 getUpdatedWorldMatrix() const
        {
            glm::mat4 T = glm::translate(glm::mat4(1.0f), m_position);
            glm::mat4 R = glm::mat4_cast(m_orientation);
            glm::mat4 S = glm::scale(glm::mat4(1.0f), m_scale);

            return T * R * S;
        }

    private:
        std::vector<TransformComponent*> m_children;

        glm::mat4 m_worldMatrix{};
        glm::mat3 m_normalMatrix{};

        glm::quat m_orientation{};
        glm::vec3 m_position{};
        glm::vec3 m_rotation{};
        glm::vec3 m_scale{1.0f};
        glm::vec3 m_direction{};
        
        bool m_isDirty = true;
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

        bool isInitiallyActivated = false;

    protected:
        void* runtimeBody = nullptr;

    private:
        friend class PhysicsSystem;
    };

    struct SphereColliderComponent
    {
        SphereColliderComponent() = default;
        SphereColliderComponent(float            radius, 
                                const glm::vec3& offset = { 0.0f, 0.0f, 0.0f }) 
            : radius(radius),
              offset(offset) {}

        glm::vec3 offset = { 0.0f, 0.0f, 0.0f };
        float     radius = 0.5f;
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
}