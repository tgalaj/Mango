#pragma once

#include "Scene.h"
#include "Camera.h"
#include "Raytracer.h"
#include "Options.h"
#include <thread>
#include <atomic>

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
    void process(Scene& scene, int left, int right);
    std::vector<int> thread_bounds(int parts, int mem) const;

    Options m_options;

    glm::vec3 * m_framebuffer;
    Camera * m_cam;
    Raytracer * m_raytarcer;

    std::vector<std::thread> m_threads;
    std::atomic<int> m_processed_pixel_counter;
};
