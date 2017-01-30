#pragma once
#include "Texture.h"

enum class MaterialType { DIFFUSE, REFLECTION, REFLECTION_AND_REFRACTION };

struct Material
{
    Texture* m_textrue           = nullptr;
    glm::vec3 m_albedo           = glm::vec3(0.18f);
    MaterialType m_type          = MaterialType::DIFFUSE;
    float m_index_of_refreaction = 1.0f;
    float m_ka                   = 1.0f, 
          m_kd                   = 1.0f, 
          m_ks                   = 1.0f;
    float m_specular_exponent    = 20.0f;
    bool use_flat_shading        = false;

    Material()
    {
        
    }

    Material(const Material & other)
    {
        m_albedo = other.m_albedo;
        m_type = other.m_type;
        m_index_of_refreaction = other.m_index_of_refreaction;
        m_ka = other.m_ka;
        m_kd = other.m_kd;
        m_ks = other.m_ks;
        m_specular_exponent = other.m_specular_exponent;
        use_flat_shading = other.use_flat_shading;

        if (other.m_textrue != nullptr)
        {
            m_textrue = new Texture();
            m_textrue->load(other.m_textrue->m_name);
            m_textrue->use_linear = other.m_textrue->use_linear;
        }
    }

    Material& operator=(const Material& other)
    {
        m_albedo = other.m_albedo;
        m_type = other.m_type;
        m_index_of_refreaction = other.m_index_of_refreaction;
        m_ka = other.m_ka;
        m_kd = other.m_kd;
        m_ks = other.m_ks;
        m_specular_exponent = other.m_specular_exponent;
        use_flat_shading = other.use_flat_shading;

        if (other.m_textrue != nullptr)
        {
            m_textrue = new Texture();
            m_textrue->load(other.m_textrue->m_name);
            m_textrue->use_linear = other.m_textrue->use_linear;
        }

        return *this;
    }

    ~Material()
    {
        if (m_textrue != nullptr)
        {
            delete m_textrue;
            m_textrue = nullptr;
        }
    }
};
