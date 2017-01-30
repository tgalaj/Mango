#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

Texture::Texture()
    : m_width(0),
      m_height(0), 
      use_linear(false),
      data(nullptr)
{
}

Texture::~Texture()
{
    if(data != nullptr)
    {
        delete[] data;
    }
}

bool Texture::load(const std::string & file_name)
{
    if(file_name.empty())
    {
        return false;
    }

    bool is_loaded = false;
    int width, height, components;

    unsigned char* pixels = stbi_load(("res/textures/" + file_name).c_str(), &width, &height, &components, 4);

    m_name = file_name;

    if(pixels != nullptr)
    {
        m_width = width;
        m_height = height;

        data = new glm::vec3[m_width * m_height];

        for(uint32_t i = 0; i < m_width * m_height; ++i)
        {
            auto r = pixels[i * 4 + 0];
            auto g = pixels[i * 4 + 1];
            auto b = pixels[i * 4 + 2];
            //auto a = pixels[i * 4 + 3];

            data[i] = glm::vec3(r, g, b) / 255.0f;
        }

        is_loaded = true;
    }
    else
    {
        std::cout << "Could not load file " << file_name << std::endl;
    }

    stbi_image_free(pixels);

    return is_loaded;
}

glm::vec3 Texture::fetchPixel(float u, float v) const
{
    u = 1.0f - u;

    if(use_linear)
    {
        u *= m_width  - 1.0f;
        v *= m_height - 1.0f;

        int x = std::floor(u);
        int y = std::floor(v);

        float u_ratio = u - x;
        float v_ratio = v - y;
        float u_opposite = 1 - u_ratio;
        float v_opposite = 1 - v_ratio;

        return (data[x * m_width + y] * u_opposite + data[(x + 1) * m_width + y] * u_ratio) * v_opposite +
               (data[x * m_width + y + 1] * u_opposite + data[(x + 1) * m_width + y + 1] * u_ratio) * v_ratio;

    }
    else
    {
        int _x = u * (m_width - 1.0f) + 0.5f;
        int _y = v * (m_height - 1.0f) + 0.5f;

        int index = _x * m_width + _y;
        return data[index];
    }
}
