#pragma once
#include <FreeImage.h>
#include "Raytracer.h"
#include <thread>
#include <atomic>

class Framebuffer
{
public:
    Framebuffer(int width, int height);
    ~Framebuffer();

    void raytrace(Scene& scene, const std::string& scene_file_name);

    bool use_tone_mapping = true;

private:
    const int BPP = 24; //cause three 8 bit RGB values

    std::vector<std::thread> m_threads;
    std::atomic<int> m_processed_pixel_counter;

    FIBITMAP* m_image;

    int m_width;
    int m_height;
    int m_size;

    void process(Scene& scene, int left, int right);
    std::vector<int> thread_bounds(int parts, int mem) const;
};
