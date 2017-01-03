#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include "Scene.h"
#include "Camera.h"
#include "Raytracer.h"

struct Options
{
    std::string output_file_name = "output";
    uint32_t width               = 640;
    uint32_t height              = 480;
    float fov                    = 60.0f;
    glm::vec3 cam_origin         = glm::vec3(0.0f);
};

class Framebuffer
{
public:
    Framebuffer(const Options & options);
    ~Framebuffer();

    void render(const Scene & scene);

private:
    void savePPM() const;

    Options m_options;

    glm::vec3 * m_framebuffer;
    Camera * m_cam;
    Raytracer * m_raytarcer;
};
