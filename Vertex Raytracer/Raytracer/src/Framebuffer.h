#pragma once
#include <FreeImage.h>
#include "Raytracer.h"
#include <thread>

class Framebuffer
{
public:
    Framebuffer(int width, int height);
    ~Framebuffer();

    void raytrace(Scene& scene, const std::string& scene_file_name);

private:
    const int BPP = 24; //cause three 8 bit RGB values

    std::vector<std::thread> m_threads;
    FIBITMAP* m_image;

    int m_width;
    int m_height;

    void process(Scene& scene, int left, int right);
    std::vector<int> thread_bounds(int parts, int mem) const;
};
