#include "mgpch.h"

#include "glm/gtc/type_ptr.hpp"

#include "Shader.h"
#include "CoreEngine/CoreAssetManager.h"
#include "Utilities/Util.h"
#include "Utilities/ShaderGlobals.h"
#include "CoreEngine/CoreServices.h"

namespace mango
{
    Shader::Shader()
        : m_program_id(0),
          m_is_linked(false)
    {
        m_program_id = glCreateProgram();

        if (m_program_id == 0)
        {
            fprintf(stderr, "Error while creating program object.\n");
        }
    }

    Shader::Shader(const std::string & compute_shader_filename)
        : Shader()
    {
        addShader(compute_shader_filename, GL_COMPUTE_SHADER);
    }

    Shader::Shader(const std::string & vertex_shader_filename,
                   const std::string & fragment_shader_filename)
        : Shader()
    {
        addShader(vertex_shader_filename, GL_VERTEX_SHADER);
        addShader(fragment_shader_filename, GL_FRAGMENT_SHADER);
    }

    Shader::Shader(const std::string & vertex_shader_filename,
                   const std::string & fragment_shader_filename,
                   const std::string & geometry_shader_filename)
        : Shader(vertex_shader_filename, fragment_shader_filename)
    {
        addShader(geometry_shader_filename, GL_GEOMETRY_SHADER);
    }

    Shader::Shader(const std::string & vertex_shader_filename,
                   const std::string & fragment_shader_filename,
                   const std::string & tessellation_control_shader_filename,
                   const std::string & tessellation_evaluation_shader_filename)
        : Shader(vertex_shader_filename, fragment_shader_filename)
    {
        addShader(tessellation_control_shader_filename, GL_TESS_CONTROL_SHADER);
        addShader(tessellation_evaluation_shader_filename, GL_TESS_EVALUATION_SHADER);
    }

    Shader::Shader(const std::string & vertex_shader_filename,
                   const std::string & fragment_shader_filename,
                   const std::string & geometry_shader_filename,
                   const std::string & tessellation_control_shader_filename,
                   const std::string & tessellation_evaluation_shader_filename)
        : Shader(vertex_shader_filename,
                 fragment_shader_filename,
                 tessellation_control_shader_filename,
                 tessellation_evaluation_shader_filename)
    {
        addShader(geometry_shader_filename, GL_GEOMETRY_SHADER);
    }

    Shader::~Shader()
    {
        if (m_program_id != 0)
        {
            glDeleteProgram(m_program_id);
            m_program_id = 0;
        }
    }

    void Shader::addShader(std::string const & file_name, GLuint type) const
    {
        if (m_program_id == 0)
        {
            return;
        }

        if (file_name.empty())
        {
            fprintf(stderr, "Error: Shader's file name can't be empty.\n");

            return;
        }

        GLuint shaderObject = glCreateShader(type);

        if (shaderObject == 0)
        {
            fprintf(stderr, "Error while creating %s.\n", file_name.c_str());

            return;
        }


        std::string code = Util::loadFile("assets/shaders/" + file_name);
        code = Util::loadShaderIncludes(code);

        const char * shader_code = code.c_str();

        glShaderSource(shaderObject, 1, &shader_code, nullptr);
        glCompileShader(shaderObject);

        GLint result;
        glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);

        if (result == GL_FALSE)
        {
            fprintf(stderr, "%s compilation failed!\n", file_name.c_str());

            GLint logLen;
            glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &logLen);

            if (logLen > 0)
            {
                char * log = static_cast<char *>(malloc(logLen));

                GLsizei written;
                glGetShaderInfoLog(shaderObject, logLen, &written, log);

                fprintf(stderr, "Shader log: \n%s", log);
                free(log);
            }
            getchar();
            return;
        }

        glAttachShader(m_program_id, shaderObject);
        glDeleteShader(shaderObject);
    }

    void Shader::addAllUniforms()
    {
        /* Get all active uniforms except uniforms in blocks */
        GLenum program_interface = GL_UNIFORM;
        GLint num_uniforms = 0;
        glGetProgramInterfaceiv(m_program_id, program_interface, GL_ACTIVE_RESOURCES, &num_uniforms);

        const GLenum properties[] = { GL_BLOCK_INDEX, GL_TYPE, GL_NAME_LENGTH, GL_LOCATION };
        const GLint properties_size = sizeof(properties) / sizeof(properties[0]);

        for(GLint i = 0; i < num_uniforms; ++i)
        {
            GLint values[properties_size];
            glGetProgramResourceiv(m_program_id, program_interface, i, properties_size, properties, properties_size, nullptr, values);

            /* Skip all uniforms in blocks */
            if(values[0] != -1)
            {
                continue;
            }

            std::vector<char> name_data(values[2]);
            glGetProgramResourceName(m_program_id, program_interface, i, name_data.size(), nullptr, &name_data[0]);
            std::string uniform_name(name_data.begin(), name_data.end() - 1);

            std::string prefix = uniform_name.substr(0, 2);
            if(GLOBAL_UNIFORM_PREFIX == prefix)
            {
                m_global_uniforms_types.push_back(values[1]);
                m_global_uniforms_names.push_back(uniform_name);
                m_uniforms_locations[uniform_name] = values[3];
            }
            else if(SKIP_UNIFORM_PREFIX == prefix)
            {
                m_uniforms_locations[uniform_name] = values[3];
            }
            else
            {
                m_uniforms_types.push_back(values[1]);
                m_uniforms_names.push_back(uniform_name);
                m_uniforms_locations[uniform_name] = values[3];
            }
        }
    }

    void Shader::addAllSubroutines()
    {
        GLenum interfaces[]    = { GL_VERTEX_SUBROUTINE, GL_FRAGMENT_SUBROUTINE };
        GLenum shader_stages[] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };

        GLint interfaces_count = sizeof(interfaces) / sizeof(interfaces[0]);

        for(GLint i = 0; i < interfaces_count; ++i)
        {
            /* Get all active subroutines */
            GLenum program_interface = interfaces[i];

            GLint num_subroutines = 0;
            glGetProgramInterfaceiv(m_program_id, program_interface, GL_ACTIVE_RESOURCES, &num_subroutines);

            const GLenum properties[] = { GL_NAME_LENGTH };
            const GLint properties_size = sizeof(properties) / sizeof(properties[0]);

            GLint count_subroutine_locations = 0;
            glGetProgramStageiv(m_program_id, shader_stages[i], GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &count_subroutine_locations);
            m_active_subroutine_uniform_locations[shader_stages[i]] = count_subroutine_locations;

            for (GLint j = 0; j < num_subroutines; ++j)
            {
                GLint values[properties_size];
                GLint length = 0;
                glGetProgramResourceiv(m_program_id, program_interface, j, properties_size, properties, properties_size, &length, values);

                std::vector<char> name_data(values[0]);
                glGetProgramResourceName(m_program_id, program_interface, j, name_data.size(), nullptr, &name_data[0]);
                std::string subroutine_name(name_data.begin(), name_data.end() - 1);

                GLuint subroutine_index = glGetSubroutineIndex(m_program_id, shader_stages[i], subroutine_name.c_str());

                m_subroutine_indices[subroutine_name] = subroutine_index;
            }
        }
    }

    bool Shader::link()
    {
        glLinkProgram(m_program_id);

        GLint status;
        glGetProgramiv(m_program_id, GL_LINK_STATUS, &status);

        if (status == GL_FALSE)
        {
            fprintf(stderr, "Failed to link shader program!\n");

            GLint logLen;
            glGetProgramiv(m_program_id, GL_INFO_LOG_LENGTH, &logLen);

            if (logLen > 0)
            {
                char* log = (char*)malloc(logLen);
                GLsizei written;
                glGetProgramInfoLog(m_program_id, logLen, &written, log);

                fprintf(stderr, "Program log: \n%s", log);
                free(log);
            }
        }
        else
        {
            m_is_linked = true;

            addAllUniforms();
            addAllSubroutines();
        }

        return m_is_linked;
    }

    void Shader::bind() const
    {
        if (m_program_id != 0 && m_is_linked)
        {
            glUseProgram(m_program_id);
        }
    }

    void Shader::updateUniforms(Material & material)
    {
        for(unsigned i = 0; i < m_uniforms_names.size(); ++i)
        {
            auto uniform_name = m_uniforms_names[i];
            auto uniform_type = m_uniforms_types[i];

            switch(uniform_type)
            {
                case GL_SAMPLER_2D:
                    if(M_TEXTURE_DIFFUSE == uniform_name)
                    {
                        material.getTexture(Material::TextureType::DIFFUSE)->bind(GLuint(Material::TextureType::DIFFUSE));
                    }
                    else
                    if(M_TEXTURE_SPECULAR == uniform_name)
                    {
                        material.getTexture(Material::TextureType::SPECULAR)->bind(GLuint(Material::TextureType::SPECULAR));
                    }
                    else
                    if(M_TEXTURE_NORMAL == uniform_name)
                    {
                        material.getTexture(Material::TextureType::NORMAL)->bind(GLuint(Material::TextureType::NORMAL));
                    }
                    else
                    if (M_TEXTURE_EMISSION == uniform_name)
                    {
                        material.getTexture(Material::TextureType::EMISSION)->bind(GLuint(Material::TextureType::EMISSION));
                    }
                    else
                    if (M_TEXTURE_DEPTH == uniform_name)
                    {
                        material.getTexture(Material::TextureType::DEPTH)->bind(GLuint(Material::TextureType::DEPTH));
                    }
                    break;
                case GL_FLOAT_VEC3:
                    setUniform(uniform_name, material.getVector3(uniform_name));
                    break;
                case GL_FLOAT:
                    setUniform(uniform_name, material.getFloat(uniform_name));
                    break;
            }
        }
    }

    void Shader::updateGlobalUniforms(const TransformComponent & transform)
    {
        for(unsigned i = 0; i < m_global_uniforms_names.size(); ++i)
        {
            auto uniform_name = m_global_uniforms_names[i];
            auto uniform_type = m_global_uniforms_types[i];

            switch(uniform_type)
            {
                case GL_FLOAT_MAT4:
                {
                    if (G_MVP == uniform_name)
                    {
                        auto view = CoreServices::getRenderer()->getCamera()->m_view;
                        auto projection = CoreServices::getRenderer()->getCamera()->m_projection;

                        auto mvp = projection * view * transform.world_matrix();
                        setUniform(G_MVP, mvp);
                    }
                    else
                    if (G_MODEL_MATRIX == uniform_name)
                    {
                        setUniform(G_MODEL_MATRIX, transform.world_matrix());
                    }
                    break;
                }
                case GL_FLOAT_MAT3:
                {
                    if (G_NORMAL_MATRIX == uniform_name)
                    {
                        setUniform(G_NORMAL_MATRIX, transform.normal_matrix());
                    }
                    break;
                }
                case GL_FLOAT_VEC3:
                {
                    if (G_CAM_POS == uniform_name)
                    {
                        setUniform(G_CAM_POS, CoreServices::getRenderer()->getCameraTransform()->position());
                    }
                }
            }
        }
    }

    bool Shader::getUniformLocation(const std::string & uniform_name)
    {
        GLint uniform_location = glGetUniformLocation(m_program_id, uniform_name.c_str());

        if (uniform_location != -1)
        {
            m_uniforms_locations[uniform_name] = uniform_location;
            return true;
        }
        else
        {
            fprintf(stderr, "Error! Can't find uniform %s\n", uniform_name.c_str());
            return false;
        }
    }

    void Shader::setUniform(const std::string & uniformName, float value)
    {
        if (m_uniforms_locations.count(uniformName))
        {
            glProgramUniform1f(m_program_id, m_uniforms_locations[uniformName], value);
        }
        else
        {
            if (getUniformLocation(uniformName))
            {
                glProgramUniform1f(m_program_id, m_uniforms_locations[uniformName], value);
            }
        }
    }

    void Shader::setUniform(const std::string & uniformName, int value)
    {
        if (m_uniforms_locations.count(uniformName))
        {
            glProgramUniform1i(m_program_id, m_uniforms_locations[uniformName], value);
        }
        else
        {
            if (getUniformLocation(uniformName))
            {
                glProgramUniform1i(m_program_id, m_uniforms_locations[uniformName], value);
            }
        }
    }

    void Shader::setUniform(const std::string & uniformName, unsigned int value)
    {
        if (m_uniforms_locations.count(uniformName))
        {
            glProgramUniform1ui(m_program_id, m_uniforms_locations.at(uniformName), value);
        }
        else
        {
            if (getUniformLocation(uniformName))
            {
                glProgramUniform1ui(m_program_id, m_uniforms_locations[uniformName], value);
            }
        }
    }

    void Shader::setUniform(const std::string & uniformName, GLsizei count, float * value)
    {
        if (m_uniforms_locations.count(uniformName))
        {
            glProgramUniform1fv(m_program_id, m_uniforms_locations[uniformName], count, value);
        }
        else
        {
            if (getUniformLocation(uniformName))
            {
                glProgramUniform1fv(m_program_id, m_uniforms_locations[uniformName], count, value);
            }
        }
    }

    void Shader::setUniform(const std::string & uniformName, GLsizei count, int * value)
    {
        if (m_uniforms_locations.count(uniformName))
        {
            glProgramUniform1iv(m_program_id, m_uniforms_locations[uniformName], count, value);
        }
        else
        {
            if (getUniformLocation(uniformName))
            {
                glProgramUniform1iv(m_program_id, m_uniforms_locations[uniformName], count, value);
            }
        }
    }

    void Shader::setUniform(const std::string & uniformName, GLsizei count, glm::vec3 * vectors)
    {
        if (m_uniforms_locations.count(uniformName))
        {
            glProgramUniform3fv(m_program_id, m_uniforms_locations[uniformName], count, glm::value_ptr(vectors[0]));
        }
        else
        {
            if (getUniformLocation(uniformName))
            {
                glProgramUniform3fv(m_program_id, m_uniforms_locations[uniformName], count, glm::value_ptr(vectors[0]));
            }
        }
    }

    void Shader::setUniform(const std::string & uniformName, const glm::vec2 & vector)
    {
        if (m_uniforms_locations.count(uniformName))
        {
            glProgramUniform2fv(m_program_id, m_uniforms_locations[uniformName], 1, glm::value_ptr(vector));
        }
        else
        {
            if (getUniformLocation(uniformName))
            {
                glProgramUniform2fv(m_program_id, m_uniforms_locations[uniformName], 1, glm::value_ptr(vector));
            }
        }
    }

    void Shader::setUniform(const std::string & uniformName, const glm::vec3 & vector)
    {
        if (m_uniforms_locations.count(uniformName))
        {
            glProgramUniform3fv(m_program_id, m_uniforms_locations[uniformName], 1, glm::value_ptr(vector));
        }
        else
        {
            if (getUniformLocation(uniformName))
            {
                glProgramUniform3fv(m_program_id, m_uniforms_locations[uniformName], 1, glm::value_ptr(vector));
            }
        }
    }

    void Shader::setUniform(const std::string & uniformName, const glm::vec4 & vector)
    {
        if (m_uniforms_locations.count(uniformName))
        {
            glProgramUniform4fv(m_program_id, m_uniforms_locations[uniformName], 1, glm::value_ptr(vector));
        }
        else
        {
            if (getUniformLocation(uniformName))
            {
                glProgramUniform4fv(m_program_id, m_uniforms_locations[uniformName], 1, glm::value_ptr(vector));
            }
        }
    }

    void Shader::setUniform(const std::string & uniformName, const glm::mat3 & matrix)
    {
        if (m_uniforms_locations.count(uniformName))
        {
            glProgramUniformMatrix3fv(m_program_id, m_uniforms_locations[uniformName], 1, GL_FALSE, glm::value_ptr(matrix));
        }
        else
        {
            if (getUniformLocation(uniformName))
            {
                glProgramUniformMatrix3fv(m_program_id, m_uniforms_locations[uniformName], 1, GL_FALSE, glm::value_ptr(matrix));
            }
        }
    }

    void Shader::setUniform(const std::string & uniformName, const glm::mat4 & matrix)
    {
        if (m_uniforms_locations.count(uniformName))
        {
            glProgramUniformMatrix4fv(m_program_id, m_uniforms_locations[uniformName], 1, GL_FALSE, glm::value_ptr(matrix));
        }
        else
        {
            if (getUniformLocation(uniformName))
            {
                glProgramUniformMatrix4fv(m_program_id, m_uniforms_locations[uniformName], 1, GL_FALSE, glm::value_ptr(matrix));
            }
        }
    }

    void Shader::setUniform(const std::string & uniformName, glm::mat4 * matrices, unsigned count)
    {
        if (m_uniforms_locations.count(uniformName))
        {
            glProgramUniformMatrix4fv(m_program_id, m_uniforms_locations[uniformName], count, GL_FALSE, &matrices[0][0][0]);
        }
        else
        {
            if (getUniformLocation(uniformName))
            {
                glProgramUniformMatrix4fv(m_program_id, m_uniforms_locations[uniformName], count, GL_FALSE, &matrices[0][0][0]);
            }
        }
    }

    void Shader::setSubroutine(Type shader_type, const std::string & subroutine_name)
    {
        glUniformSubroutinesuiv(GLenum(shader_type), m_active_subroutine_uniform_locations[GLenum(shader_type)], &m_subroutine_indices[subroutine_name]);
    }
}
