#pragma once
#include <glm/glm.hpp>

struct Options
{
    std::string output_file_name = "output";
    uint32_t width = 640;
    uint32_t height = 480;
    uint32_t max_depth = 5;
    glm::vec3 background_color = glm::vec3(0.0f);//glm::vec3(0.235294f, 0.67451f, 0.843137f);
    glm::vec3 cam_origin = glm::vec3(0.0f);
    glm::vec3 cam_lookat = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cam_up = glm::vec3(0.0f, -1.0f, 0.0f);
    float fov = 60.0f;
    float shadow_bias = 1e-4f;
};
