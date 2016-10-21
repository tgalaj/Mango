#pragma once
#include <FreeImage.h>
#include "Raytracer.h"

class Framebuffer
{
public:
    Framebuffer(int width, int height);
    ~Framebuffer();

    void raytrace(Raytracer& raytracer, const std::string& scene_file_name) const;

private:
    const int BPP = 24; //cause three 8 bit RGB values

    int m_width;
    int m_height;
    FIBITMAP* m_image;
};
