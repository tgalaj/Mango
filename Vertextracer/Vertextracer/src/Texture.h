#pragma once
#include <string>
#include <glm/detail/type_vec3.hpp>

class Texture
{
public:
    Texture();
    ~Texture();

    bool load(const std::string & file_name);
    glm::highp_dvec3 fetchPixel(double u, double v) const;

    uint32_t m_width;
    uint32_t m_height;
    std::string m_name;
    bool use_linear;

private:
    glm::highp_dvec3* data;
};
