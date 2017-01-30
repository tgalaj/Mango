#pragma once
#include <string>
#include <glm/detail/type_vec3.hpp>

class Texture
{
public:
    Texture();
    ~Texture();

    bool load(const std::string & file_name);
    glm::vec3 fetchPixel(float u, float v) const;

    uint32_t m_width;
    uint32_t m_height;
    std::string m_name;
    bool use_linear;

private:
    glm::vec3* data;
};
