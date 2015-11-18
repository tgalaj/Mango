#include "Shader.h"
#include "CoreAssetManager.h"
#include "VertexEngineCore.h"
#include <glm\gtc\type_ptr.hpp>

Shader::Shader(const std::string & vertexShaderFilename,
               const std::string & fragmentShaderFilename,
               const std::string & geometryShaderFilename, 
               const std::string & tessellationControlShaderFilename, 
               const std::string & tessellationEvaluationShaderFilename, 
               const std::string & computeShaderFilename) : program_id(0), isLinked(false)
{
    std::string shaderVersion = "#version " + std::to_string(MIN_GL_VERSION_MAJOR) + std::to_string(MIN_GL_VERSION_MAJOR) + "0\n\n";

    const std::string shaderCodes[6] = { CoreAssetManager::loadFile(vertexShaderFilename), 
                                         CoreAssetManager::loadFile(fragmentShaderFilename), 
                                         CoreAssetManager::loadFile(geometryShaderFilename),
                                         CoreAssetManager::loadFile(tessellationControlShaderFilename),
                                         CoreAssetManager::loadFile(tessellationEvaluationShaderFilename),
                                         CoreAssetManager::loadFile(computeShaderFilename)
                                       };

    const std::string filenames[6] = { vertexShaderFilename, 
                                       fragmentShaderFilename, 
                                       geometryShaderFilename,
                                       tessellationControlShaderFilename,
                                       tessellationEvaluationShaderFilename,
                                       computeShaderFilename
                                     };

    /* Check if compute shader's source code is the only one available. */
    if (!shaderCodes[5].empty())
    {
        for (auto & shaderCode : shaderCodes)
        {
            if (!shaderCode.empty())
            {
                fprintf(stderr, "Compute shader can't be created due to presence of other shader source codes.\n");
                return;
            }
        }
    }

    program_id = glCreateProgram();

    if (program_id == 0)
    {
        fprintf(stderr, "Error while creating program object.\n");
        return;
    }

    for (int i = 0; i < sizeof(shaderCodes) / sizeof(std::string); ++i)
    {
        if (shaderCodes[i].empty())
        {
            continue;
        }

        GLuint shaderType = 0;
        
        if (i == 0)
            shaderType = GL_VERTEX_SHADER;
        else
        if (i == 1)
            shaderType = GL_FRAGMENT_SHADER;
        else
        if (i == 2)
            shaderType = GL_GEOMETRY_SHADER;
        else
        if (i == 3)
            shaderType = GL_TESS_CONTROL_SHADER;
        else
        if (i == 4)
            shaderType = GL_TESS_EVALUATION_SHADER;
        else
        if (i == 5)
            shaderType = GL_COMPUTE_SHADER;

        if (shaderType == 0)
        {
            fprintf(stderr, "Error! Wrong shader type.\n");
            continue;
        }

        GLuint shaderObject = glCreateShader(shaderType);

        if (shaderObject == 0)
        {
            fprintf(stderr, "Error while creating %s.\n", filenames[i].c_str());
            continue;
        }

        const char *shaderCode[2] = { shaderVersion.c_str(), shaderCodes[i].c_str() };

        glShaderSource (shaderObject, 2, shaderCode, NULL);
        glCompileShader(shaderObject);

        GLint result;
        glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);

        if (result == GL_FALSE)
        {
            fprintf(stderr, "%s compilation failed!\n", filenames[i].c_str());

            GLint logLen;
            glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &logLen);

            if (logLen > 0)
            {
                char * log = (char *)malloc(logLen);

                GLsizei written;
                glGetShaderInfoLog(shaderObject, logLen, &written, log);

                fprintf(stderr, "Shader log: \n%s", log);
                free(log);
            }

            continue;
        }

        glAttachShader(program_id, shaderObject);
        glDeleteShader(shaderObject);
    }

    link();
}

Shader::~Shader()
{
    if (program_id != 0)
    {
        glDeleteProgram(program_id);
        program_id = 0;
    }
}

bool Shader::link()
{
    glLinkProgram(program_id);

    GLint status;
    glGetProgramiv(program_id, GL_LINK_STATUS, &status);

    if (status == GL_FALSE)
    {
        fprintf(stderr, "Failed to link shader program!\n");

        GLint logLen;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &logLen);

        if (logLen > 0)
        {
            char* log = (char*)malloc(logLen);
            GLsizei written;
            glGetProgramInfoLog(program_id, logLen, &written, log);

            fprintf(stderr, "Program log: \n%s", log);
            free(log);
        }
    }
    else
    {
        isLinked = true;
    }

    return isLinked;
}

void Shader::apply()
{
    if (program_id != 0 && isLinked)
    {
        glUseProgram(program_id);
    }
}

bool Shader::getUniformLocation(const std::string & uniform_name)
{
    GLint uniform_location = glGetUniformLocation(program_id, uniform_name.c_str());
    
    if (uniform_location != -1)
    {
        uniformsLocations[uniform_name] = uniform_location;
        return true;
    }
    else
    {
        fprintf(stderr, "Error! Can't find uniform %s\n", uniform_name.c_str());
        return false;
    }
}

void Shader::setUniform1f(const std::string & uniformName, float value)
{
    if (uniformsLocations.count(uniformName))
    {
        glUniform1f(uniformsLocations[uniformName], value);
    }
    else
    {
        if (getUniformLocation(uniformName))
        {
            glUniform1f(uniformsLocations[uniformName], value);
        }
    }
}

void Shader::setUniform1i(const std::string & uniformName, int value)
{
    if (uniformsLocations.count(uniformName))
    {
        glUniform1i(uniformsLocations[uniformName], value);
    }
    else
    {
        if (getUniformLocation(uniformName))
        {
            glUniform1i(uniformsLocations[uniformName], value);
        }
    }
}

void Shader::setUniform1ui(const std::string & uniformName, unsigned int value)
{
    if (uniformsLocations.count(uniformName))
    {
        glUniform1ui(uniformsLocations.at(uniformName), value);
    }
    else
    {
        if (getUniformLocation(uniformName))
        {
            glUniform1ui(uniformsLocations[uniformName], value);
        }
    }
}

void Shader::setUniform2fv(const std::string & uniformName, glm::vec2 & vector)
{
    if (uniformsLocations.count(uniformName))
    {
        glUniform2fv(uniformsLocations[uniformName], 1, glm::value_ptr(vector));
    }
    else
    {
        if (getUniformLocation(uniformName))
        {
            glUniform2fv(uniformsLocations[uniformName], 1, glm::value_ptr(vector));
        }
    }
}

void Shader::setUniform3fv(const std::string & uniformName, glm::vec3 & vector)
{
    if (uniformsLocations.count(uniformName))
    {
        glUniform3fv(uniformsLocations[uniformName], 1, glm::value_ptr(vector));
    }
    else
    {
        if (getUniformLocation(uniformName))
        {
            glUniform3fv(uniformsLocations[uniformName], 1, glm::value_ptr(vector));
        }
    }
}

void Shader::setUniformMatrix3fv(const std::string & uniformName, glm::mat3 & matrix)
{
    if (uniformsLocations.count(uniformName))
    {
        glUniformMatrix3fv(uniformsLocations[uniformName], 1, GL_FALSE, glm::value_ptr(matrix));
    }
    else
    {
        if (getUniformLocation(uniformName))
        {
            glUniformMatrix3fv(uniformsLocations[uniformName], 1, GL_FALSE, glm::value_ptr(matrix));
        }
    }
}

void Shader::setUniformMatrix4fv(const std::string & uniformName, glm::mat4 & matrix)
{
    if (uniformsLocations.count(uniformName))
    {
        glUniformMatrix4fv(uniformsLocations[uniformName], 1, GL_FALSE, glm::value_ptr(matrix));
    }
    else
    {
        if (getUniformLocation(uniformName))
        {
            glUniformMatrix4fv(uniformsLocations[uniformName], 1, GL_FALSE, glm::value_ptr(matrix));
        }
    }
}
