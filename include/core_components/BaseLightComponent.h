#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace mango
{
    class ShadowInfo
    {
    public:
        explicit ShadowInfo(const glm::mat4 & projection = glm::mat4(1.0f),
                            bool casts_shadows = true)
            : m_projection   (projection),
              m_casts_shadows(casts_shadows){}

        ~ShadowInfo() {}

        const glm::mat4 & getProjection() const { return m_projection; }
        bool getCastsShadows() const { return m_casts_shadows; }

        void setCastsShadows(bool casts_shadows) { m_casts_shadows = casts_shadows; }

    private:
        glm::mat4 m_projection;
        bool m_casts_shadows;
    };

    class BaseLightComponent
    {
    public:
        BaseLightComponent(const glm::vec3 & color, float intensity)
            : m_color(color),
              m_intensity(intensity),
              m_shadow_info(ShadowInfo()) {}

        virtual ~BaseLightComponent() {}

        const ShadowInfo & getShadowInfo() const { return m_shadow_info; }

        void setCastsShadows(bool casts_shadows) { m_shadow_info.setCastsShadows(casts_shadows); }

        glm::vec3 m_color;
        float     m_intensity;

    protected:
        void setShadowInfo(const ShadowInfo & shadow_info) { m_shadow_info = shadow_info; }

    private:
        ShadowInfo m_shadow_info;
    };
}
