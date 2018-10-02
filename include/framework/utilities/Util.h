#pragma once
#include <string>
#include <stb_image.h>

namespace Vertex
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
        static std::string loadFile(const std::string & filename);

        static std::string loadShaderIncludes(const std::string & shader_code);
        /**
        * @brief   Loads a file that contains an image data.
        * @param   std::string Relative path, with file name
        *          and extension, to the file that needs to be
        *          loaded.
        * @returns Pointer to unsigned char that contains image's data.
        *          Have to be freed with stbi_image_free(data)!
        */
        static unsigned char* loadTexture(const std::string & filename);
    };
}