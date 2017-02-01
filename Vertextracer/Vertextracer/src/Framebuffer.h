#pragma once

#include "Scene.h"
#include "Camera.h"
#include "Raytracer.h"
#include "Options.h"
#include <thread>
#include <atomic>

#define RENDER_SINGLE 0

class Framebuffer
{
public:
    Framebuffer(const Options & options);
    ~Framebuffer();

    void render(Scene & scene);

    Options m_options;
private:
    glm::highp_dvec3 adaptive_antialias(const glm::highp_dvec2 & top_left,
                                        const glm::highp_dvec2 & top_right,
                                        const glm::highp_dvec2 & bottom_left,
                                        const glm::highp_dvec2 & bottom_right,
                                        Scene & scene,
                                        const uint32_t & depth = 0);

    glm::highp_dvec3 stochastic_antialias(const glm::highp_dvec2 & pixel, Scene & scene, int num_samples);
    void fxaa();

    void savePPM() const;
    void process(Scene& scene, int left, int right);
    std::vector<int> thread_bounds(int parts, int mem) const;

    glm::highp_dvec3 * m_framebuffer;
    Camera * m_cam;
    Raytracer * m_raytarcer;

    std::vector<std::thread> m_threads;
    std::atomic<int> m_processed_pixel_counter;
};
