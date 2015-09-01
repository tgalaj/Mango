#pragma once

#include <FreeImage.h>

#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>

class CoreAssetManager final
{
public:
    CoreAssetManager() = delete;
    virtual ~CoreAssetManager();

    /**
     * @brief   Loads a file in a text mode.
     * @param   std::string Relative path, with file name 
     *                      and extension, to the file that
     *                      needs to be loaded.
     * @returns Full file's source as a std::string.
     */
    static std::string loadFile (const std::string & filename);

    /**
     * @brief   Loads a file that contains multiple shaders 
     *          in a text mode.
     * @param   std::string Relative path, with file name
     *          and extension, to the file that needs to be 
     *          loaded.
     * @returns std::map of separate shader source codes where
     *          key is one of [VS, GS, TES, TCS, FS, CS] and 
     *          value is a corresponding shader code. If there 
     *          is no shader code for a specific stage, value 
     *          is an empty string.
     */
    static std::map<std::string, std::string> loadShaderCode(const std::string & filename);

    /**
     * @brief   Loads a file that contains an image data.
     * @param   std::string Relative path, with file name
     *          and extension, to the file that needs to be 
     *          loaded.
     * @returns Pointer to FIBITMAP that contains image's data.
     */
    static FIBITMAP* loadTexture(const std::string & filename);
};

