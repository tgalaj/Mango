#include <cstdio>

#include "Framebuffer.h"
#include "Time.h"

Framebuffer::Framebuffer(int width, int height)
    : m_width (width),
      m_height(height),
      m_image (nullptr)
{
    FreeImage_Initialise();

    m_image = FreeImage_Allocate(m_width, m_height, BPP);

    if (!m_image)
    {
        printf("ERROR: Could not allocate the image!\n");
    }
}

Framebuffer::~Framebuffer()
{
    FreeImage_DeInitialise();

    printf("Press any key to continue...\n");
    getchar();
}

void Framebuffer::raytrace(Scene& scene, const std::string& scene_file_name)
{
    /* Raytracing code */
    int num_threads = std::thread::hardware_concurrency() * 4;

    std::vector<int> bounds = thread_bounds(num_threads, m_width * m_height);

    double start_time = Time::getTime();

    /* Use num_threads - 1 threads to raytrace the scene */
    for(int i = 0; i < num_threads - 1; ++i)
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

    if (FreeImage_Save(FIF_PNG, m_image, scene_file_name.c_str(), 0))
    {
        printf("Image %s successfully saved!\n", scene_file_name.c_str());
        printf("Processing time = %.2fs\n", Time::getTime() - start_time);
    }
}

void Framebuffer::process(Scene& scene, int left, int right)
{
    RGBQUAD color;
    Raytracer raytracer(scene);

    for (int i = left; i < right; ++i) 
    {
        int ii = i / m_width;
        int jj = i - ii * m_width;

        glm::vec3 c = raytracer.illuminate(jj, ii);

        color.rgbRed   = static_cast<BYTE>(c.r * 255.0f + 0.5f);
        color.rgbGreen = static_cast<BYTE>(c.g * 255.0f + 0.5f);
        color.rgbBlue  = static_cast<BYTE>(c.b * 255.0f + 0.5f);

        FreeImage_SetPixelColor(m_image, m_width - jj, ii, &color);
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
