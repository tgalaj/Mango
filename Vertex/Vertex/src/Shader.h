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

    void setUniform1f       (const std::string & uniformName, float value);
    void setUniform1i       (const std::string & uniformName, int value);
    void setUniform1ui      (const std::string & uniformName, unsigned int value);
    void setUniform2fv      (const std::string & uniformName, glm::vec2 & vector);
    void setUniform3fv      (const std::string & uniformName, glm::vec3 & vector);
    void setUniformMatrix3fv(const std::string & uniformName, glm::mat3 & matrix);
    void setUniformMatrix4fv(const std::string & uniformName, glm::mat4 & matrix);

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
    bool getUniformLocation(const std::string & uniform_name);
};

