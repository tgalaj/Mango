#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

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
            Ray primary_ray = m_cam->getPrimaryRay(x, y);

            //Atmospheric scattering part
            if (scene.atmosphere != nullptr)
            {
                float t0, t1, tMax = std::numeric_limits<float>::max();
                if (scene.atmosphere->intersect(primary_ray, t0, t1, true) && t1 > 0.0f)
                {
                    tMax = std::max(0.0f, t0);
                }
                
                *(pixel) = scene.atmosphere->computeIncidentLight(primary_ray, 0, tMax);
            }

            *(pixel++) += m_raytarcer->castRay(primary_ray, scene, m_options); //TODO proper blending
        }
        fprintf(stderr, "\b\b\b\b%3d%c", int(100 * y / (m_options.height - 1)), '%');
    }

    std::cout << "\rRaytracing time = " << Time::getTime() - start_time << "s" <<  std::endl;
    std::cout << "Saving PPM output...\n";

    savePPM();
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
