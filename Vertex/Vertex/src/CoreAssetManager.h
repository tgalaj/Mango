#pragma once

#include <FreeImage.h>

#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>

#include "Model.h"

class CoreAssetManager final
{
public:
    friend class Shader;
    friend class Texture;
    friend class VertexEngineCore;

    /**
     * @brief   Loads a file in a text mode.
     * @param   std::string Relative path, with file name 
     *                      and extension, to the file that
     *                      needs to be loaded.
     * @returns Full file's source as a std::string.
     */
    static const std::string loadFile (const std::string & filename);

    static Texture * const createTexture2D     (const std::string & filename,  GLint base_level = 0);
    static Texture * const createCubeMapTexture(const std::string * filenames, GLint base_level = 0);

    static Model   * const createModel(const std::string & filename);
    static Model   * const createModel();

private:
    CoreAssetManager() {};
    ~CoreAssetManager();

    static std::map<std::string, Texture *> loadedTextures;
    static std::map<std::string, Model *> loadedModels;

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

