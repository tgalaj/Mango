#pragma once

#include "Shader.h"
#include <map>

class ShaderManager final
{
public:
    friend class VertexEngineCore;

    ShaderManager(const ShaderManager &)             = delete;
    ShaderManager & operator=(const ShaderManager &) = delete;

    static void createShader(const std::string & shaderName,
                             const std::string & vertexShaderFilename,
                             const std::string & fragmentShaderFilename,
                             const std::string & geometryShaderFilename               = "",
                             const std::string & tessellationControlShaderFilename    = "",
                             const std::string & tessellationEvaluationShaderFilename = "",
                             const std::string & computeShaderFilename                = "");

    static Shader * const getShader(const std::string & shaderName);

private:
    ShaderManager() {};
    ~ShaderManager();

    static std::map<std::string, Shader *> shaders;
};