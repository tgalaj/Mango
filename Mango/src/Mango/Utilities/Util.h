#pragma once

#include "Mango/Rendering/Texture.h"

#include <filesystem>
#include <stb_image.h>
#include <string>

namespace mango
{
    class Util
    {
    public:
        /**
        * @brief   Loads a file in a text mode.
        * @param   std::string Relative path, with file name
        *                      and extension, to the file that
        *                      needs to be loaded.
        * @returns Full file's source as a std::string.
        */
        static std::string loadFile(const std::filesystem::path & filepath);

        static std::string loadShaderIncludes(const std::string & shaderCode);

        /**
        * @brief   Loads a file that contains an image data.
        * @param   std::string Relative path, with file name
        *          and extension, to the file that needs to be
        *          loaded.
        * @param   image_data
        * @returns Pointer to unsigned char that contains image's data.
        *          Have to be freed with stbi_image_free(data)!
        */
        static unsigned char* loadTexture(const std::filesystem::path & filepath, ImageData & imageData);
    };
}