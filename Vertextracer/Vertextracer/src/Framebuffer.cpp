#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include <glm/gtc/epsilon.hpp>

#include "Framebuffer.h"
#include "Ray.h"
#include "Time.h"

Framebuffer::Framebuffer(const Options & options)
{
    m_options = options;

    m_framebuffer = new glm::vec3[m_options.width * m_options.height];
    memset(m_framebuffer, 0, sizeof(glm::vec3) * m_options.width * m_options.height);

    m_cam = new Camera(m_options.width, 
                       m_options.height, 
                       m_options.fov,
                       m_options.cam_origin,
                       m_options.cam_lookat,
                       m_options.cam_up);

    m_raytarcer = new Raytracer();
}

Framebuffer::~Framebuffer()
{
    if(m_framebuffer != nullptr)
    {
        delete[] m_framebuffer;
    }

    if (m_cam != nullptr)
    {
        delete m_cam;
    }

    if (m_raytarcer != nullptr)
    {
        delete m_raytarcer;
    }
}

void Framebuffer::render(Scene & scene)
{
    glm::vec3 * pixel = m_framebuffer;
    int counter = 0;

    int processed_pixel_counter = 0;
    auto start_time             = Time::getTime();

    for(uint32_t y = 0; y < m_options.height; ++y)
    {
        for(uint32_t x = 0; x < m_options.width; ++x)
        {
            if(m_options.enable_antialiasing)
            {
                *(pixel++) += antialias(glm::vec2(x, y),
                                        glm::vec2(x + 1.0f, y), 
                                        glm::vec2(x, y + 1.0f), 
                                        glm::vec2(x + 1.0f, y + 1.0f), 
                                        scene);
            }
            else
            {
                Ray primary_ray = m_cam->getPrimaryRay(x + 0.5f, y + 0.5f);
                
                *(pixel++) += m_raytarcer->castRay(primary_ray, scene, m_options);
            }
        }
        fprintf(stderr, "\b\b\b\b%3d%c", int(100 * y / (m_options.height - 1)), '%');
    }

    std::cout << "\rRaytracing time = " << Time::getTime() - start_time << "s" <<  std::endl;
    std::cout << "Saving PPM output...\n";

    savePPM();
}

glm::vec3 Framebuffer::antialias(glm::vec2 top_left, glm::vec2 top_right, glm::vec2 bottom_left, glm::vec2 bottom_right, Scene & scene, const uint32_t & depth)
{
    Ray primary_ray_TL = m_cam->getPrimaryRay(top_left.x,     top_left.y);
    Ray primary_ray_TR = m_cam->getPrimaryRay(top_right.x,    top_right.y);
    Ray primary_ray_BL = m_cam->getPrimaryRay(bottom_left.x,  bottom_left.y);
    Ray primary_ray_BR = m_cam->getPrimaryRay(bottom_right.x, bottom_right.y);
    
    glm::vec3 pixel_TL = m_raytarcer->castRay(primary_ray_TL, scene, m_options);
    glm::vec3 pixel_TR = m_raytarcer->castRay(primary_ray_TR, scene, m_options);
    glm::vec3 pixel_BL = m_raytarcer->castRay(primary_ray_BL, scene, m_options);
    glm::vec3 pixel_BR = m_raytarcer->castRay(primary_ray_BR, scene, m_options);

    bool should_divide = false;

    if(!glm::all(glm::epsilonEqual(pixel_TL, pixel_TR, m_options.aa_epsilon)))
    {
        should_divide = true;
    }
    else if(!glm::all(glm::epsilonEqual(pixel_TL, pixel_BL, m_options.aa_epsilon)))
    {
        should_divide = true;
    }
    else if(!glm::all(glm::epsilonEqual(pixel_TL, pixel_BR, m_options.aa_epsilon)))
    {
        should_divide = true;
    }

    if(should_divide)
    {
        glm::vec2 mid        = (top_left    + bottom_right) / 2.0f;
        glm::vec2 top_mid    = (top_left    + top_right)    / 2.0f;
        glm::vec2 left_mid   = (top_left    + bottom_left)  / 2.0f;
        glm::vec2 right_mid  = (top_right   + bottom_right) / 2.0f;
        glm::vec2 bottom_mid = (bottom_left + bottom_right) / 2.0f;

        if(depth + 1 < m_options.aa_max_depth)
        {
            pixel_TL = antialias(top_left, top_mid,   left_mid,    mid,          scene, depth + 1);
            pixel_TR = antialias(top_mid,  top_right, mid,         right_mid,    scene, depth + 1);
            pixel_BL = antialias(left_mid, mid,       bottom_left, bottom_mid,   scene, depth + 1);
            pixel_BR = antialias(mid,      right_mid, bottom_mid,  bottom_right, scene, depth + 1);
        }
    }

    return (pixel_TL + pixel_TR + pixel_BL + pixel_BR) / 4.0f;
}

void Framebuffer::savePPM() const
{
    std::ofstream output(m_options.output_file_name + ".ppm", std::ios::out | std::ios::binary);

    output << "P6\n" << m_options.width << " " << m_options.height << "\n255\n";

    for(uint32_t i = 0; i < m_options.width * m_options.height; ++i)
    {
        #if 1
        // Apply tone mapping function
        m_framebuffer[i].r = m_framebuffer[i].r < 1.413f ? glm::pow(m_framebuffer[i].r * 0.38317f, 1.0f / 2.2f) : 1.0f - glm::exp(-m_framebuffer[i].r);
        m_framebuffer[i].g = m_framebuffer[i].g < 1.413f ? glm::pow(m_framebuffer[i].g * 0.38317f, 1.0f / 2.2f) : 1.0f - glm::exp(-m_framebuffer[i].g);
        m_framebuffer[i].b = m_framebuffer[i].b < 1.413f ? glm::pow(m_framebuffer[i].b * 0.38317f, 1.0f / 2.2f) : 1.0f - glm::exp(-m_framebuffer[i].b);
        #endif

        char r = char(255 * glm::clamp(m_framebuffer[i].r, 0.0f, 1.0f));
        char g = char(255 * glm::clamp(m_framebuffer[i].g, 0.0f, 1.0f));;
        char b = char(255 * glm::clamp(m_framebuffer[i].b, 0.0f, 1.0f));;

        output << r << g << b;
    }

    output.close();

    std::cout << "Opening PPM " << m_options.output_file_name + ".ppm\n";
    system((m_options.output_file_name + ".ppm").c_str());
}
