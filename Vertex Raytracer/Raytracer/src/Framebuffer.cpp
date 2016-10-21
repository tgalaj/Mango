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

void Framebuffer::raytrace(Raytracer& raytracer, const std::string& scene_file_name) const
{
    /* Raytracing code */
    int total_pixels = m_width * m_height;
    int pixels_counter = 0;

    RGBQUAD color;

    double start_time = Time::getTime();

    for (int i = 0; i < m_height; ++i)
    {
        for (int j = 0; j < m_width; ++j)
        {
            glm::vec3 c = raytracer.illuminate(j, i);

            color.rgbRed   = static_cast<BYTE>(c.r * 255.0f + 0.5f);
            color.rgbGreen = static_cast<BYTE>(c.g * 255.0f + 0.5f);
            color.rgbBlue  = static_cast<BYTE>(c.b * 255.0f + 0.5f);

            FreeImage_SetPixelColor(m_image, m_width - j, i, &color);

            ++pixels_counter;
            if (pixels_counter % 1000 == 0)
                printf("%d / %d pixels \r", pixels_counter, total_pixels);
        }
    }

    if (FreeImage_Save(FIF_PNG, m_image, scene_file_name.c_str(), 0))
    {
        printf("Image %s successfully saved!\n", scene_file_name.c_str());
        printf("Processing time = %.2fs\n", Time::getTime() - start_time);
    }
}
