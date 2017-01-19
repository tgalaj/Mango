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
    void savePPM() const;

    Options m_options;

    glm::vec3 * m_framebuffer;
    Camera * m_cam;
    Raytracer * m_raytarcer;
};
