#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>

#include <map>
#include <string>

class Shader
{
public:
    friend class Model;
    friend class Renderer;

    Shader(std::string const & filename);
    ~Shader();

    void setUniformMatrix4fv(std::string uniformName, glm::mat4 & matrix);

protected:

private:
    GLuint program_id;
    bool isLinked;

    bool link();
    void apply();
};

