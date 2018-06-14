#pragma once
#include <string>
#include <FreeImage.h>

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
        * @returns Pointer to FIBITMAP that contains image's data.
        */
        static FIBITMAP* loadTexture(const std::string & filename);
    };
}