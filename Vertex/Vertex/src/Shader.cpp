#include "Shader.h"
#include "CoreAssetManager.h"
#include "VertexEngineCore.h"

Shader::Shader(std::string const &filename) : program_id(0), isLinked(false)
{
    std::string shaderVersion = "#version " + std::to_string(MIN_GL_VERSION_MAJOR) + std::to_string(MIN_GL_VERSION_MAJOR) + "0\n\n";

    auto shaderCodes = CoreAssetManager::loadShaderCode(filename);

    program_id = glCreateProgram();

    if (program_id == 0)
    {
        fprintf(stderr, "Error creating program object.\n");
        return;
    }

    for (auto &shader : shaderCodes)
    {
        GLuint shaderType = 0;
        
        if (shader.first == "CS")
            shaderType = GL_COMPUTE_SHADER;
        else
        if (shader.first == "FS")
            shaderType = GL_FRAGMENT_SHADER;
        else
        if (shader.first == "GS")
            shaderType = GL_GEOMETRY_SHADER;
        else
        if (shader.first == "TCS")
            shaderType = GL_TESS_CONTROL_SHADER;
        else
        if (shader.first == "TES")
            shaderType = GL_TESS_EVALUATION_SHADER;
        else
        if (shader.first == "VS")
            shaderType = GL_VERTEX_SHADER;

        if (shaderType == 0)
        {
            fprintf(stderr, "Error! Wrong shader type.\n");
            continue;
        }

        GLuint shaderObject = glCreateShader(shaderType);

        if (shaderObject == 0)
        {
            fprintf(stderr, "Error creating %s.\n", filename.c_str());
            continue;
        }

        if (shader.second.empty())
        {
            continue;
        }

        const char *shaderCode[2] = { shaderVersion.c_str(), shader.second.c_str() };

        glShaderSource (shaderObject, 2, shaderCode, NULL);
        glCompileShader(shaderObject);

        GLint result;
        glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);

        if (result == GL_FALSE)
        {
            fprintf(stderr, "%s compilation failed!\n", filename.c_str());

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

        //std::cout << "Successfully attached " << shader.first << " to a program!\n";
    }
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
