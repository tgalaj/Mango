#pragma once

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
#include <vector>

namespace mango
{
    class ShadowInfo
    {
    public:
        explicit ShadowInfo(const glm::mat4& projection = glm::mat4(1.0f), bool castsShadows = true)
            : m_projection  (projection),
              m_castsShadows(castsShadows) {}

        ~ShadowInfo() {}

        const glm::mat4& getProjection()   const { return m_projection; }
        bool             getCastsShadows() const { return m_castsShadows; }

        void setCastsShadows(bool castsShadows) { m_castsShadows = castsShadows; }

    private:
        glm::mat4 m_projection{};
        bool      m_castsShadows;
    };

    class BaseLightComponent
    {
    public:
        BaseLightComponent(const glm::vec3& color, float intensity)
            : color       (color),
              intensity   (intensity),
              m_shadowInfo(ShadowInfo()) {}

        virtual ~BaseLightComponent() {}

        const ShadowInfo& getShadowInfo() const { return m_shadowInfo; }

        void setCastsShadows(bool castsShadows) { m_shadowInfo.setCastsShadows(castsShadows); }

        glm::vec3 color{};
        float     intensity;

    protected:
        void setShadowInfo(const ShadowInfo& shadowInfo) { m_shadowInfo = shadowInfo; }

    private:
        ShadowInfo m_shadowInfo;
    };

    class DirectionalLightComponent : public BaseLightComponent
    {
    public:
        DirectionalLightComponent(const glm::vec3 & color, float intensity, float size = 20.0f)
            : BaseLightComponent(color, intensity)
        {
            setShadowInfo(ShadowInfo(glm::ortho(-size, size, -size, size, -size, size), true));
        }

        explicit DirectionalLightComponent()
            : BaseLightComponent(glm::vec3(1.0f), 1.0f)
        {}
    };

    class PointLightComponent : public BaseLightComponent
    {
    public:
        PointLightComponent()
            : BaseLightComponent(glm::vec3(1.0f), 1.0f),
              attenuation       (0.0, 0.0f, 1.0f)
        {
            calculateRange();
            setShadowInfo(ShadowInfo(glm::perspective(glm::radians(90.0f), 1.0f, 0.001f, 100.0f), true));
        }

        PointLightComponent(const glm::vec3 &   color, 
                                  float         intensity, 
                            const Attenuation & attenuation)
            : BaseLightComponent(color, intensity),
              attenuation       (attenuation)
        {
            calculateRange();
            setShadowInfo(ShadowInfo(glm::perspective(glm::radians(90.0f), 1.0f, 0.001f, 100.0f), true));
        }

        void setAttenuation(float constant, float linear, float quadratic)
        {
            attenuation = Attenuation(constant, linear, quadratic);
            calculateRange();
        }

        Attenuation attenuation;
        float       range{};

    private:
        static const int COLOR_DEPTH = 255;

        void calculateRange()
        {
            float a = attenuation.quadratic;
            float b = attenuation.linear;
            float c = attenuation.constant - COLOR_DEPTH * intensity * glm::compMax(color);

            range = (-b + glm::sqrt(b * b - 4 * a * c)) / (2 * a);
        }
    };

    class SpotLightComponent : public PointLightComponent
    {
    public:
        SpotLightComponent()
            : m_cutoff(0.3f)
        {
            setShadowInfo(ShadowInfo(glm::perspective(glm::acos(m_cutoff), 1.0f, 0.001f, 100.0f), true));
        }

        SpotLightComponent(const glm::vec3 & color, float intensity, const Attenuation & attenuation, float cutoff)
            : PointLightComponent(color, intensity, attenuation),
              m_cutoff(cutoff)
        {
            setShadowInfo(ShadowInfo(glm::perspective(glm::acos(m_cutoff), 1.0f, 0.001f, 100.0f), true));
        }

        /* 
         * Set angle in degrees. 
         */
        void setCutOffAngle(float angle)
        {
            m_cutoff = glm::cos(glm::radians(angle));
        }

        /* 
         * Returns cosine of cutoff angle. 
         */
        float getCutOffAngle() const { return m_cutoff; }

    private:
        float m_cutoff;
    };

    class CameraComponent
    {
    public:
        CameraComponent()
            : m_isOrtho(false)
        {
        }

        /*
         * Perspective camera
         */
        CameraComponent(float fov, 
                        float aspectRatio, 
                        float zNear, 
                        float zFar);

        /*
         * Ortho camera
         */
        CameraComponent(float left, 
                        float right, 
                        float bottom, 
                        float top,
                        float zNear, 
                        float zFar);

        bool isPrimary() const { return m_isPrimary; }
        void setPrimary();

        const glm::mat4& view()       const { return m_view; }
        const glm::mat4& projection() const { return m_projection; }

        void setView      (const glm::mat4& view)      { m_view       = view; }
        void setProjection(const glm::mat4 projection) { m_projection = projection; };

     private:
        glm::mat4 m_view{};
        glm::mat4 m_projection{};

        bool m_isOrtho;
        bool m_isPrimary = true;
    
    private:
        friend class Scene;
    };

    class ModelRendererComponent
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

    class TransformComponent
    {
    public:
        explicit TransformComponent(const glm::vec3 & position    = glm::vec3(0.0f),
                                    const glm::vec3 & orientation = glm::vec3(0.0f, 0.0f, 0.0f),
                                    const glm::vec3 & scale       = glm::vec3(1.0f))
            : m_orientation (orientation),
              m_position    (position),
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
         * Set orientation using Euler Angles in degrees
         */
        
        void setOrientation(float x, float y, float z)
        {
            m_orientation = glm::angleAxis(glm::radians(x), glm::vec3(1.0f, 0.0f, 0.0f)) *
                            glm::angleAxis(glm::radians(y), glm::vec3(0.0f, 1.0f, 0.0f)) *
                            glm::angleAxis(glm::radians(z), glm::vec3(0.0f, 0.0f, 1.0f));
            m_direction   = glm::normalize(glm::conjugate(m_orientation) * glm::vec3(0.0f, 0.0f, 1.0f));
            m_isDirty     = true;
        }

        /*
        * Set orientation using axis and angle in degrees
        */
        void setOrientation(const glm::vec3 & axis, float angle)
        {
            m_orientation = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
            m_direction   = glm::normalize(glm::conjugate(m_orientation) * glm::vec3(0.0f, 0.0f, 1.0f));
            m_isDirty    = true;
        }

        void setOrientation(const glm::quat & quat)
        {
            m_orientation = quat;
            m_direction   = glm::normalize(glm::conjugate(m_orientation) * glm::vec3(0.0f, 0.0f, 1.0f));
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
        glm::vec3 m_scale{};
        glm::vec3 m_direction{};
        
        bool m_isDirty;
    };
}