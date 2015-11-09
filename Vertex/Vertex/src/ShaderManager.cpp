#include "ShaderManager.h"

std::map<std::string, Shader *> ShaderManager::shaders;

ShaderManager::~ShaderManager()
{
    for (auto it = shaders.begin(); it != shaders.end(); ++it)
    {
        delete it->second;
        it->second = nullptr;
    }

    shaders.clear();
}

void ShaderManager::createShader(const std::string & shaderName, 
                                 const std::string & vertexShaderFilename, 
                                 const std::string & fragmentShaderFilename, 
                                 const std::string & geometryShaderFilename,
                                 const std::string & tessellationControlShaderFilename, 
                                 const std::string & tessellationEvaluationShaderFilename, 
                                 const std::string & computeShaderFilename)
{
    if(!shaders.count(shaderName))
    {
        shaders[shaderName] = new Shader(vertexShaderFilename,
                                         fragmentShaderFilename,
                                         geometryShaderFilename,
                                         tessellationControlShaderFilename,
                                         tessellationEvaluationShaderFilename,
                                         computeShaderFilename);
    }
}

Shader * const ShaderManager::getShader(const std::string & shaderName)
{
    if(shaders.count(shaderName))
    {
        return shaders.at(shaderName);
    }
    else
    {
        return nullptr;
    }
}
