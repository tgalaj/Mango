#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace mango
{
    class ShadowInfo
    {
    public:
        explicit ShadowInfo(const glm::mat4 & projection = glm::mat4(1.0f), bool castsShadows = true)
            : m_projection  (projection),
              m_castsShadows(castsShadows){}

        ~ShadowInfo() {}

        const glm::mat4 & getProjection()   const { return m_projection; }
        bool              getCastsShadows() const { return m_castsShadows; }

        void setCastsShadows(bool castsShadows) { m_castsShadows = castsShadows; }

    private:
        glm::mat4 m_projection{};
        bool      m_castsShadows;
    };

    class BaseLightComponent
    {
    public:
        BaseLightComponent(const glm::vec3 & color, float intensity)
            : color       (color),
              intensity   (intensity),
              m_shadowInfo(ShadowInfo()) {}

        virtual ~BaseLightComponent() {}

        const ShadowInfo & getShadowInfo() const { return m_shadowInfo; }

        void setCastsShadows(bool castsShadows) { m_shadowInfo.setCastsShadows(castsShadows); }

        glm::vec3 color{};
        float     intensity;

    protected:
        void setShadowInfo(const ShadowInfo & shadowInfo) { m_shadowInfo = shadowInfo; }

    private:
        ShadowInfo m_shadowInfo;
    };
}
