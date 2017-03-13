#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include <glm/gtc/epsilon.hpp>

#include "Framebuffer.h"
#include "Ray.h"
#include "Time.h"
#include <random>

Framebuffer::Framebuffer(const Options & options)
{
    m_options = options;

    m_framebuffer = new glm::highp_dvec3[m_options.WIDTH * m_options.HEIGHT];
    memset(m_framebuffer, 0, sizeof(glm::highp_dvec3) * m_options.WIDTH * m_options.HEIGHT);

    if(m_options.CAM_PITCH > 89.0f)
    {
        m_options.CAM_PITCH = 89.0f;
    }
    if (m_options.CAM_PITCH < -89.0f)
    {
        m_options.CAM_PITCH = -89.0f;
    }

    glm::highp_dvec3 cam_dir;
    cam_dir.x = glm::cos(glm::radians(m_options.CAM_YAW)) * glm::cos(glm::radians(m_options.CAM_PITCH));
    cam_dir.y = glm::sin(glm::radians(m_options.CAM_PITCH));
    cam_dir.z = glm::sin(glm::radians(m_options.CAM_YAW)) * glm::cos(glm::radians(m_options.CAM_PITCH));
    cam_dir   = glm::normalize(cam_dir);

    glm::highp_dvec3 cam_right;
    cam_right.x = glm::cos(glm::radians(m_options.CAM_YAW) - glm::half_pi<double>());
    cam_right.y = 0.0f;
    cam_right.z = glm::sin(glm::radians(m_options.CAM_YAW) - glm::half_pi<double>());;
    cam_right   = glm::normalize(cam_right);
    m_options.CAM_UP = glm::cross(cam_right, cam_dir);

    m_cam = new Camera(m_options.WIDTH, 
                       m_options.HEIGHT, 
                       m_options.CAM_FOV,
                       m_options.CAM_ORIGIN,
                       m_options.CAM_ORIGIN + cam_dir,
                       m_options.CAM_UP);

    m_raytarcer = new Raytracer();
    m_processed_pixel_counter = 0;
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
    int num_threads         = std::thread::hardware_concurrency();
    std::vector<int> bounds = thread_bounds(num_threads, m_options.HEIGHT);
    auto start_time         = Time::getTime();

    /* Use num_threads - 1 threads to raytrace the scene */
    for (int i = 0; i < num_threads - 1; ++i)
    {
        m_threads.push_back(std::thread(&Framebuffer::process, this, std::ref(scene), bounds[i], bounds[i + 1]));
    }

    /* Use main thread to raytrace the scene */
    for (int i = num_threads - 1; i < num_threads; ++i)
    {
        process(scene, bounds[i], bounds[i + 1]);
    }

    for (auto &t : m_threads)
    {
        t.join();
    }

    if(m_options.ANTYALIASING_TYPE == Options::AAType::FXAA)
    {
        for (uint32_t i = 0; i < m_options.FXAA_NUM_ITERATIONS; ++i)
        {
            fxaa();
        }
    }

    if (!m_options.RENDER_MULTI_FRAMES)
    {
        std::cout << "\rRaytracing time = " << Time::getTime() - start_time << "s" << std::endl;
        std::cout << "Saving PPM output...\n";
    }

    savePPM();

    memset(m_framebuffer, 0, sizeof(glm::highp_dvec3) * m_options.WIDTH * m_options.HEIGHT);
    m_threads.clear();
    m_processed_pixel_counter = 0;
}

void Framebuffer::process(Scene & scene, int left, int right)
{
    for (int y = left; y < right; ++y)
    {
        for (uint32_t x = 0; x < m_options.WIDTH; ++x)
        {
            if (m_options.ENABLE_ANTIALIASING && m_options.ANTYALIASING_TYPE != Options::AAType::FXAA)
            {
                switch(m_options.ANTYALIASING_TYPE)
                {
                    case Options::AAType::ADAPTIVE:
                    {
                        m_framebuffer[x + y * m_options.WIDTH] += adaptive_antialias(glm::highp_dvec2(x, y),
                                                                                     glm::highp_dvec2(x + 1.0f, y),
                                                                                     glm::highp_dvec2(x, y + 1.0f),
                                                                                     glm::highp_dvec2(x + 1.0f, y + 1.0f),
                                                                                     scene);
                        break;
                    }
                    case Options::AAType::STOCHASTIC:
                    {
                        m_framebuffer[x + y * m_options.WIDTH] += stochastic_antialias(glm::highp_dvec2(x, y),
                                                                                       scene,
                                                                                       m_options.STOCHASTIC_AA_NUM_SAMPLES);
                        break;
                    }
                }
            }
            else
            {
                Ray primary_ray = m_cam->getPrimaryRay(x + 0.5f, y + 0.5f);

                m_framebuffer[x + y * m_options.WIDTH] += m_raytarcer->castRay(primary_ray, scene, m_options);
            }

            m_processed_pixel_counter += 1;

            if (m_processed_pixel_counter % 100 == 0)
            {
                printf(" %.1f%c\r", double(m_processed_pixel_counter.load()) / double(m_options.WIDTH * m_options.HEIGHT) * 100.0f, '%');
            }
        }
    }
}

std::vector<int> Framebuffer::thread_bounds(int parts, int mem) const
{
    std::vector<int> bounds;
    int delta = mem / parts;
    int reminder = mem % parts;
    int N1 = 0, N2 = 0;
    bounds.push_back(N1);

    for (int i = 0; i < parts; ++i)
    {
        N2 = N1 + delta;

        if (i == parts - 1)
        {
            N2 += reminder;
        }

        bounds.push_back(N2);
        N1 = N2;
    }

    return bounds;
}

glm::highp_dvec3 Framebuffer::adaptive_antialias(const glm::highp_dvec2 & top_left, 
                                                 const glm::highp_dvec2 & top_right, 
                                                 const glm::highp_dvec2 & bottom_left, 
                                                 const glm::highp_dvec2 & bottom_right, 
                                                 Scene & scene, 
                                                 const uint32_t & depth)
{
    Ray primary_ray_TL = m_cam->getPrimaryRay(top_left.x,     top_left.y);
    Ray primary_ray_TR = m_cam->getPrimaryRay(top_right.x,    top_right.y);
    Ray primary_ray_BL = m_cam->getPrimaryRay(bottom_left.x,  bottom_left.y);
    Ray primary_ray_BR = m_cam->getPrimaryRay(bottom_right.x, bottom_right.y);
    
    glm::highp_dvec3 pixel_TL = m_raytarcer->castRay(primary_ray_TL, scene, m_options);
    glm::highp_dvec3 pixel_TR = m_raytarcer->castRay(primary_ray_TR, scene, m_options);
    glm::highp_dvec3 pixel_BL = m_raytarcer->castRay(primary_ray_BL, scene, m_options);
    glm::highp_dvec3 pixel_BR = m_raytarcer->castRay(primary_ray_BR, scene, m_options);

    bool should_divide = false;

    if(!glm::all(glm::epsilonEqual(pixel_TL, pixel_TR, m_options.ADAPTIVE_AA_EPSILON)))
    {
        should_divide = true;
    }
    else if(!glm::all(glm::epsilonEqual(pixel_TL, pixel_BL, m_options.ADAPTIVE_AA_EPSILON)))
    {
        should_divide = true;
    }
    else if(!glm::all(glm::epsilonEqual(pixel_TL, pixel_BR, m_options.ADAPTIVE_AA_EPSILON)))
    {
        should_divide = true;
    }

    if(should_divide)
    {
        glm::highp_dvec2 mid        = (top_left    + bottom_right) / 2.0;
        glm::highp_dvec2 top_mid    = (top_left    + top_right)    / 2.0;
        glm::highp_dvec2 left_mid   = (top_left    + bottom_left)  / 2.0;
        glm::highp_dvec2 right_mid  = (top_right   + bottom_right) / 2.0;
        glm::highp_dvec2 bottom_mid = (bottom_left + bottom_right) / 2.0;

        if(depth + 1 < m_options.ADAPTIVE_AA_MAX_DEPTH)
        {
            pixel_TL = adaptive_antialias(top_left, top_mid,   left_mid,    mid,          scene, depth + 1);
            pixel_TR = adaptive_antialias(top_mid,  top_right, mid,         right_mid,    scene, depth + 1);
            pixel_BL = adaptive_antialias(left_mid, mid,       bottom_left, bottom_mid,   scene, depth + 1);
            pixel_BR = adaptive_antialias(mid,      right_mid, bottom_mid,  bottom_right, scene, depth + 1);
        }
    }

    return (pixel_TL + pixel_TR + pixel_BL + pixel_BR) / 4.0;
}

glm::highp_dvec3 Framebuffer::stochastic_antialias(const glm::highp_dvec2 & pixel, Scene & scene, int num_samples)
{
    static std::default_random_engine generator;
    static std::uniform_real_distribution<double> distribution(0, 1);

    glm::highp_dvec3 p(0.0f);
    for(int i = 0; i < num_samples; ++i)
    {
        for(int j = 0; j < num_samples; ++j)
        {
            double dx = (i + distribution(generator)) / num_samples;
            double dy = (j + distribution(generator)) / num_samples;

            Ray primary_ray_TL = m_cam->getPrimaryRay(pixel.x + dx, pixel.y + dy);

            p += m_raytarcer->castRay(primary_ray_TL, scene, m_options);
        }
    }

    return p * (1.0 / double(num_samples * num_samples));
}

void Framebuffer::fxaa()
{
    glm::highp_dvec3 * texture = new glm::highp_dvec3[m_options.WIDTH * m_options.HEIGHT];
    std::copy(m_framebuffer, m_framebuffer + (m_options.WIDTH * m_options.HEIGHT), texture);

    glm::highp_dvec3 luma(0.299, 0.587, 0.114);

    double FXAA_SPAN_MAX   = m_options.FXAA_SPAN_MAX;
    double FXAA_REDUCE_MIN = m_options.FXAA_REDUCE_MIN;
    double FXAA_REDUCE_MUL = m_options.FXAA_REDUCE_MUL;

    glm::ivec2 min_clamp(0);
    glm::ivec2 max_clamp(m_options.WIDTH - 1, m_options.HEIGHT - 1);

    for(uint32_t y = 0; y < m_options.HEIGHT; ++y)
    {
        for(uint32_t x = 0; x < m_options.WIDTH; ++x)
        {
            /* Color to luminance */
            glm::ivec2 M  = glm::ivec2(x, y);
            glm::ivec2 TL = glm::clamp(M + glm::ivec2(-1, -1), min_clamp, max_clamp);
            glm::ivec2 TR = glm::clamp(M + glm::ivec2( 1, -1), min_clamp, max_clamp);
            glm::ivec2 BL = glm::clamp(M + glm::ivec2(-1,  1), min_clamp, max_clamp);
            glm::ivec2 BR = glm::clamp(M + glm::ivec2( 1,  1), min_clamp, max_clamp);

            double lumaTL = glm::dot(luma, texture[TL.x + m_options.WIDTH * TL.y]);
            double lumaTR = glm::dot(luma, texture[TR.x + m_options.WIDTH * TR.y]);
            double lumaBL = glm::dot(luma, texture[BL.x + m_options.WIDTH * BL.y]);
            double lumaBR = glm::dot(luma, texture[BR.x + m_options.WIDTH * BR.y]);
            double lumaM  = glm::dot(luma, texture[M.x  + m_options.WIDTH * M.y]);

            /* Get blur direction */
            glm::highp_dvec2 dir;
            dir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
            dir.y =  ((lumaTL + lumaBL) - (lumaTR + lumaBR));

            double dir_reduce = glm::max((lumaTL + lumaTR + lumaBL + lumaBR) * 0.25 * FXAA_REDUCE_MUL, FXAA_REDUCE_MIN);
            double inverse_dir_adjustment = 1.0 / (glm::min(glm::abs(dir.x), glm::abs(dir.y)) + dir_reduce);

            dir = glm::clamp(dir * inverse_dir_adjustment, glm::highp_dvec2(-FXAA_SPAN_MAX), glm::highp_dvec2(FXAA_SPAN_MAX));

            /* Blur */
            glm::ivec2 samp1 = glm::clamp(M + glm::ivec2(dir * glm::highp_dvec2(1.0 / 3.0 - 0.5)), min_clamp, max_clamp);
            glm::ivec2 samp2 = glm::clamp(M + glm::ivec2(dir * glm::highp_dvec2(2.0 / 3.0 - 0.5)), min_clamp, max_clamp);
            glm::highp_dvec3 result1 = 0.5 * (texture[samp1.x + m_options.WIDTH * samp1.y] + texture[samp2.x + m_options.WIDTH * samp2.y]);

            samp1 = glm::clamp(M + glm::ivec2(dir * glm::highp_dvec2(0.0 / 3.0 - 0.5)), min_clamp, max_clamp);
            samp2 = glm::clamp(M + glm::ivec2(dir * glm::highp_dvec2(3.0 / 3.0 - 0.5)), min_clamp, max_clamp);
            glm::highp_dvec3 result2 = result1 * 0.5 + 0.25 * (texture[samp1.x + m_options.WIDTH * samp1.y] + texture[samp2.x + m_options.WIDTH * samp2.y]);

            double luma_min = glm::min(lumaM, glm::min(glm::min(lumaTL, lumaTR), glm::min(lumaBL, lumaBR)));
            double luma_max = glm::max(lumaM, glm::max(glm::max(lumaTL, lumaTR), glm::max(lumaBL, lumaBR)));
            double luma_res_2 = glm::dot(luma, result2);

            /* Make sure that luminance is not out of its range */
            if(luma_res_2 < luma_min || luma_res_2 > luma_max)
            {
                m_framebuffer[M.x + m_options.WIDTH * M.y] = glm::highp_dvec3(result1);
            }
            else
            {
                m_framebuffer[M.x + m_options.WIDTH * M.y] = glm::highp_dvec3(result2);
            }
        }
    }

    delete[] texture;
}

void Framebuffer::savePPM() const
{
    std::ofstream output(m_options.OUTPUT_FILE_NAME + ".ppm", std::ios::out | std::ios::binary);

    output << "P6\n" << m_options.WIDTH << " " << m_options.HEIGHT << "\n255\n";

    for(uint32_t i = 0; i < m_options.WIDTH * m_options.HEIGHT; ++i)
    {
        #if 1
        // Apply tone mapping function
        m_framebuffer[i].r = m_framebuffer[i].r < 1.413f ? glm::pow(m_framebuffer[i].r * 0.38317f, 1.0f / 2.2f) : 1.0f - glm::exp(-m_framebuffer[i].r);
        m_framebuffer[i].g = m_framebuffer[i].g < 1.413f ? glm::pow(m_framebuffer[i].g * 0.38317f, 1.0f / 2.2f) : 1.0f - glm::exp(-m_framebuffer[i].g);
        m_framebuffer[i].b = m_framebuffer[i].b < 1.413f ? glm::pow(m_framebuffer[i].b * 0.38317f, 1.0f / 2.2f) : 1.0f - glm::exp(-m_framebuffer[i].b);
        #endif

        char r = char(255 * glm::clamp(m_framebuffer[i].r, 0.0, 1.0));
        char g = char(255 * glm::clamp(m_framebuffer[i].g, 0.0, 1.0));;
        char b = char(255 * glm::clamp(m_framebuffer[i].b, 0.0, 1.0));;

        output << r << g << b;
    }

    output.close();

    if (!m_options.RENDER_MULTI_FRAMES)
    {
        std::cout << "Opening PPM " << m_options.OUTPUT_FILE_NAME + ".ppm\n";
        system((m_options.OUTPUT_FILE_NAME + ".ppm").c_str());
    }
}
