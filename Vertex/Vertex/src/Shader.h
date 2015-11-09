#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>

#include <map>
#include <string>

class Shader
{
public:
    friend class Renderer;
    friend class ShaderManager;

    virtual ~Shader();

    void setUniformMatrix4fv(std::string uniformName, glm::mat4 & matrix);

protected:
    Shader(const std::string & vertexShaderFilename,
           const std::string & fragmentShaderFilename,
           const std::string & geometryShaderFilename               = "",
           const std::string & tessellationControlShaderFilename    = "",
           const std::string & tessellationEvaluationShaderFilename = "",
           const std::string & computeShaderFilename                = "");

private:
    std::map<std::string, GLint> uniformsLocations;

    GLuint program_id;
    bool isLinked;

    bool link();
    void apply();
    void setupUniforms(const std::string * shadersSourceCodes, unsigned int count);
};

