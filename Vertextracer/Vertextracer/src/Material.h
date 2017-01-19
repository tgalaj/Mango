#pragma once

enum class MaterialType { DIFFUSE, REFLECTION, REFLECTION_AND_REFRACTION };

struct Material
{
    glm::vec3 m_albedo = glm::vec3(0.18f);
    MaterialType m_type = MaterialType::DIFFUSE;
    float m_index_of_refreaction = 1.0f;
    float m_ka = 1.0f, m_kd = 1.0f, m_ks = 1.0f;
    float m_specular_exponent = 20.0f;
    bool use_flat_shading = false;
};
