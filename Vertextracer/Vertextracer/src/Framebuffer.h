#pragma once

#include "Scene.h"
#include "Camera.h"
#include "Raytracer.h"
#include "Options.h"

class Framebuffer
{
public:
    Framebuffer(const Options & options);
    ~Framebuffer();

    void render(Scene & scene);

private:
    glm::vec3 antialias(glm::vec2 top_left,
                        glm::vec2 top_right,
                        glm::vec2 bottom_left,
                        glm::vec2 bottom_right,
                        Scene & scene,
                        const uint32_t & depth = 0);

    void savePPM() const;

    Options m_options;

    glm::vec3 * m_framebuffer;
    Camera * m_cam;
    Raytracer * m_raytarcer;
};
